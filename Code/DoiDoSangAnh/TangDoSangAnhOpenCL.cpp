#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp> 
#include <CL/cl.hpp>
#include <iostream>
#include <vector>

// OpenCL kernel để tăng độ sáng ảnh
const char* brightnessKernel = R"CLC(
__kernel void brighten(
    __global uchar* inputImage, 
    __global uchar* outputImage, 
    const int width, 
    const int height, 
    const int step,
    const int brightness
) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x < width && y < height) {
        int index = y * step + x * 3; // BGR: mỗi pixel 3 kênh

        // Tăng độ sáng và giới hạn trong [0, 255]
        outputImage[index]     = clamp(inputImage[index] + brightness, 0, 255);
        outputImage[index + 1] = clamp(inputImage[index + 1] + brightness, 0, 255);
        outputImage[index + 2] = clamp(inputImage[index + 2] + brightness, 0, 255);
    }
}
)CLC";

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    // Đọc ảnh gốc từ thiết bị
    cv::Mat original = cv::imread("Resources/test3.jpg");
    if (original.empty()) {
        std::cerr << "Không thể đọc ảnh!" << std::endl;
        return -1;
    }

    // Chỉnh kích cỡ ảnh hiển thị
    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Original Image", original.cols / 2, original.rows / 2);
    cv::imshow("Original Image", original);

    // Chọn độ sáng muốn thêm
    int brightness;
    std::cout << "Nhập độ sáng muốn thêm: (số âm để giảm sáng): ";
    std::cin >> brightness;

    // 1. Thiết lập OpenCL
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms[0];

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];

    cl::Context context(device);
    cl::CommandQueue queue(context, device, CL_QUEUE_PROFILING_ENABLE); // Bật profiling để đo thời gian
    cl::Program::Sources sources;
    sources.push_back({ brightnessKernel, strlen(brightnessKernel) });
    cl::Program program(context, sources);

    if (program.build({ device }) != CL_SUCCESS) {
        std::cerr << "Lỗi biên dịch kernel!" << std::endl;
        return -1;
    }

    // 2. Chuẩn bị dữ liệu ảnh
    cl::Buffer inputBuffer(context, CL_MEM_READ_ONLY, original.total() * original.elemSize());
    cl::Buffer outputBuffer(context, CL_MEM_WRITE_ONLY, original.total() * original.elemSize());

    queue.enqueueWriteBuffer(inputBuffer, CL_TRUE, 0, original.total() * original.elemSize(), original.data);

    // 3. Thiết lập kernel
    cl::Kernel kernel(program, "brighten");
    kernel.setArg(0, inputBuffer);
    kernel.setArg(1, outputBuffer);
    kernel.setArg(2, original.cols);
    kernel.setArg(3, original.rows);
    kernel.setArg(4, static_cast<int>(original.step));
    kernel.setArg(5, brightness);

    // 4. Chạy kernel và đo thời gian
    cl::Event event; // Event để đo thời gian
    cl::NDRange globalSize(original.cols, original.rows);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange, nullptr, &event);

    // Chờ kernel hoàn thành
    event.wait();

    // 5. Lấy thông tin thời gian từ event
    cl_ulong timeStart, timeEnd;
    event.getProfilingInfo(CL_PROFILING_COMMAND_START, &timeStart);
    event.getProfilingInfo(CL_PROFILING_COMMAND_END, &timeEnd);
    double elapsedTime = (timeEnd - timeStart) / 1000000.0; // Chuyển nanosecond sang millisecond

    std::cout << "Thời gian thực thi kernel: " << elapsedTime << " ms" << std::endl;

    // 6. Đọc kết quả từ GPU
    cv::Mat brightened = original.clone();
    queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, original.total() * original.elemSize(), brightened.data);

    // 7. Hiển thị và lưu kết quả

    // Chỉnh kích cỡ ảnh hiển thị
    cv::namedWindow("Brightened Image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Brightened Image", original.cols / 2, original.rows / 2);
    cv::imshow("Brightened Image", brightened);

    //Tải ảnh đã chỉnh sửa về máy
    //cv::imwrite("brightened_output.png", brightened);

    cv::waitKey(0);
    return 0;
}

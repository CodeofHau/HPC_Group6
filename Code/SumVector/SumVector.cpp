#include <CL/cl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

string loadKernel(string filename) {
	ifstream file(filename);
	ostringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int main() {
	int size = 99999;
	int data1[99999];
	int data2[99999];

	// Tạo ngẫu nhiên 2 mảng có 99999 phần tử 
	for (int i = 0; i < size; i++) {
		data1[i] = std::rand() % 100;
	}

	for (int i = 0; i < size; i++) {
		data2[i] = std::rand() % 100;
	}

	// Lấy platform và device (GPU)
	cl_platform_id platform;
	cl_device_id device;
	clGetPlatformIDs(1, &platform, nullptr);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

	// Tạo context và queue
	cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr);
	cl_command_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, properties, nullptr);

	// Tạo 2 buffer chứa 2 mảng
	cl_mem buffer1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * size, data1, nullptr);
	cl_mem buffer2 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * size, data2, nullptr);

	// Đọc các hàm viết bằng OpenCL trong file kernel.cl và tạo program
	string kernelCode = loadKernel("kernel.cl");
	const char* source = kernelCode.c_str();
	cl_program program = clCreateProgramWithSource(context, 1, &source, nullptr, nullptr);
	clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);

	// Tạo kernel và truyền tham số
	cl_kernel kernel = clCreateKernel(program, "addArray", nullptr);
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer1);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer2);

	// Tạo vùng nhớ làm việc và thiết lập event để đo thời gian
	size_t global_work_size = size;
	cl_event event;

	clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work_size, nullptr, 0, nullptr, &event);

	clWaitForEvents(1, &event);
	cl_ulong startTime, endTime;
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startTime, nullptr);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, nullptr);

	// Tính toán thời gian thực thi
	double executionTime = (endTime - startTime) * 1e-6; // Chuyển từ nanoseconds sang milliseconds
	cout << "Thời gian thực thi kernel: " << executionTime << " ms" << endl;

	// Đọc bộ nhớ buffer1 và lưu lại vào mảng data1
	clEnqueueReadBuffer(queue, buffer1, CL_TRUE, 0, sizeof(int) * size, data1, 0, nullptr, nullptr);

	cout << "Kết quả sau khi cộng 2 mảng int:\n";
	for (int i = 0; i < size; i++) {
		cout << data1[i] << " ";
	}
	cout << endl;

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(buffer1);
	clReleaseMemObject(buffer2);
	clReleaseEvent(event);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);


	return 0;
}
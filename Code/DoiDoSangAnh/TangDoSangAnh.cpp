//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include<omp.h>
//
//int main() {
//    // Đọc ảnh gốc
//    std::string path = "Resources/test4.jpg";
//    cv::Mat original = cv::imread(path);
//    if (original.empty()) {
//        std::cout << "Không thể đọc ảnh!" << std::endl;
//        return -1;
//    }
//
//    // Sao chép dữ liệu ảnh gốc
//    cv::Mat brightened = original.clone();
//
//    // Tăng độ sáng bằng cách cộng một giá trị vào tất cả các pixel
//    
//    int brightness = 50; // Giá trị tăng độ sáng
//    double start = omp_get_wtime();
//    brightened += cv::Scalar(brightness, brightness, brightness);
//    double end = omp_get_wtime();
//
//    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
//    cv::resizeWindow("Original Image", original.cols / 2, original.rows / 2);
//    cv::namedWindow("Brightened Image", cv::WINDOW_NORMAL);
//    cv::resizeWindow("Brightened Image", original.cols / 2, original.rows / 2);
//
//    // Hiển thị ảnh gốc và ảnh đã tăng độ sáng
//    std::cout << "Thời gian thực thi: " << (end - start) * 1000 << std::endl;
//    cv::imshow("Original Image", original);          // Ảnh gốc không bị thay đổi
//    cv::imshow("Brightened Image", brightened);      // Ảnh đã tăng độ sáng
//
//    // Lưu ảnh nếu cần
//    //cv::imwrite("brightened_output.png", brightened);
//
//    cv::waitKey(0);
//    return 0;
//}

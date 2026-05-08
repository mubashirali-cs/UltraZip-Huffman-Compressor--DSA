# 🗜️ UltraZip: Huffman File Compressor

A lightweight, high-efficiency file compression utility written in standard C++. 

UltraZip is designed as an educational "White Box" tool to demonstrate the practical application of Data Structures and Algorithms. Unlike commercial archivers that hide their logic, UltraZip processes data streams transparently, utilizing the **Huffman Coding** (Greedy Algorithm) to reduce file redundancy without data loss.

## ✨ Features
* **100% Lossless Compression:** Reconstructs the exact bit-for-bit original file.
* **Custom CLI Dashboard:** Features a responsive Windows command-line interface with a real-time progress bar.
* **Algorithmic Transparency:** Built without STL containers like `std::priority_queue` to demonstrate raw pointer manipulation and memory management.
* **Robust Error Handling:** Defends against invalid paths, empty files, and corrupted archives.
* **Memory Safe:** Implements recursive post-order tree deletion to guarantee zero memory leaks.

## 📸 Screenshots
<img width="1920" height="1039" alt="image" src="https://github.com/user-attachments/assets/3d00b2d4-1220-48b3-9982-86f1725950da" />

<img width="1920" height="1077" alt="image" src="https://github.com/user-attachments/assets/f16fcc50-df73-4826-94da-e9b121aa08bd" />

<img width="1920" height="1043" alt="image" src="https://github.com/user-attachments/assets/7cf30427-b5ac-463f-b120-65a3dc98f4b6" />

* **Main Menu:** `![Menu](link-to-menu-image.png)`
* **Real-time Processing:** `![Progress](link-to-progress-image.png)`
* **Compression Report:** `![Stats](link-to-stats-image.png)`

## 🚀 Getting Started

### Prerequisites
* A Windows OS (The UI leverages `<windows.h>` for console styling).
* Any standard C++ compiler (GCC, MinGW, MSVC).

### Installation & Build
Since this project has zero external dependencies, you can compile it directly from the terminal or using an IDE like Visual Studio.



#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX 
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>   
#include <windows.h>
#include <vector>
#include <iomanip>

using namespace std;

// 1. PROFESSIONAL UI CLASS

class UI {
public:
    // Color Constants
    static const int HEADER_COLOR = 11;  // Cyan
    static const int TEXT_COLOR = 7;     // White
    static const int SUCCESS_COLOR = 10; // Green
    static const int ERROR_COLOR = 12;   // Red
    static const int ACCENT_COLOR = 14;  // Yellow

    static void setConsoleColor(int colorCode) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
    }

    // Draws the Logo and Boxed Title
    static void printHeader(string title) {
        system("cls"); // Clear screen
        setConsoleColor(HEADER_COLOR);

        // ASCII Art Logo
        cout << R"(
   __  ____  __________  ___   ______  __
  / / / / / /_  __/ __ \/   | /_  / / / /
 / / / / /   / / / /_/ / /| |  / / / / / 
/ /_/ / /___/ / / _, _/ ___ | / /_/_/_/  
\____/_____/_/ /_/ |_/_/  |_|/___(_|_)   
        )" << "\n";

        // Draw Box Title
        cout << "\n";
        cout << "   " << char(201); for (int i = 0; i < 42; i++) cout << char(205); cout << char(187) << "\n"; // Top Border
        cout << "   " << char(186) << "    " << title;
        for (int i = 0; i < 38 - title.length(); i++) cout << " ";
        cout << char(186) << "\n";
        cout << "   " << char(200); for (int i = 0; i < 42; i++) cout << char(205); cout << char(188) << "\n\n"; // Bottom Border

        setConsoleColor(TEXT_COLOR);
    }

    // The Progress Bar
    static void showProgress(long long current, long long total) {
        if (total == 0) return;

        // Only update occasionally to save CPU
        int percent = (int)((current * 100) / total);
        if (current % (total / 100 + 1) != 0 && current != total) return;

        cout << "\r   Processing: [";
        setConsoleColor(ACCENT_COLOR);

        int bars = percent / 5; // 20 bars total
        for (int i = 0; i < 20; i++) {
            if (i < bars) cout << char(219); // Full block
            else cout << char(176);          // Light shade
        }

        setConsoleColor(TEXT_COLOR);
        cout << "] " << percent << "%";
    }

    // Dashboard Report Card
    static void printStats(long long original, long long compressed, double time) {
        cout << "\n\n";
        setConsoleColor(HEADER_COLOR);

        // Top Border
        cout << "   " << char(218); for (int i = 0; i < 42; i++) cout << char(196); cout << char(191) << "\n";

        // Title
        cout << "   " << char(179) << "           COMPRESSION REPORT             " << char(179) << "\n";
        cout << "   " << char(195); for (int i = 0; i < 42; i++) cout << char(196); cout << char(180) << "\n";

        // Helper to print rows
        auto printRow = [](string label, string val) {
            cout << "   " << char(179) << " " << label;
            for (int i = 0; i < 20 - label.length(); i++) cout << " ";
            cout << ": " << val;
            for (int i = 0; i < 18 - val.length(); i++) cout << " ";
            cout << char(179) << "\n";
            };

        printRow("Original Size", to_string(original) + " Bytes");
        printRow("Compressed Size", to_string(compressed) + " Bytes");

        // Calculate Savings
        if (original > 0) {
            float ratio = 100.0f - ((float)compressed / original * 100.0f);
            char buff[20]; sprintf(buff, "%.2f%%", ratio);
            printRow("Space Saved", string(buff));
        }

        char timeBuff[20]; sprintf(timeBuff, "%.4f sec", time);
        printRow("Time Taken", string(timeBuff));

        // Bottom Border
        cout << "   " << char(192); for (int i = 0; i < 42; i++) cout << char(196); cout << char(217) << "\n";
        setConsoleColor(TEXT_COLOR);
    }

    // Safe Input Handler
    static string getValidPath() {
        string path;
        while (true) {
            setConsoleColor(ACCENT_COLOR);
            cout << "   >> Enter File Path: ";
            setConsoleColor(TEXT_COLOR);
            getline(cin, path);

            // Clean quotes if user copied path as "C:\..."
            if (path.length() >= 2 && path[0] == '"') {
                path = path.substr(1, path.length() - 2);
            }

            // Verify file exists
            ifstream checkFile(path);
            if (checkFile) {
                checkFile.close();
                return path;
            }
            else {
                setConsoleColor(ERROR_COLOR);
                cout << "   [!] File not found. Please try again.\n";
                setConsoleColor(TEXT_COLOR);
            }
        }
    }
};


// 2. HUFFMAN TREE STRUCTURE

struct Node {
    unsigned char character;
    int frequency;
    Node* left;
    Node* right;

    Node(unsigned char c, int f) {
        character = c;
        frequency = f;
        left = nullptr;
        right = nullptr;
    }
};


// 3. THE ALGORITHM (HUFFMAN ENGINE)

class HuffmanEngine {
public:
    Node* buildTree(int freq[256]) {
        Node* heap[512];
        int heapSize = 0;

        for (int i = 0; i < 256; i++) {
            if (freq[i] > 0) {
                heap[heapSize] = new Node((unsigned char)i, freq[i]);
                heapSize++;
            }
        }

        sortHeap(heap, heapSize);

        if (heapSize == 0) return nullptr;
        if (heapSize == 1) {
            Node* root = new Node(0, heap[0]->frequency);
            root->left = heap[0];
            return root;
        }

        while (heapSize > 1) {
            Node* smallest1 = heap[heapSize - 1];
            heapSize--;

            Node* smallest2 = heap[heapSize - 1];
            heapSize--;

            Node* parent = new Node('$', smallest1->frequency + smallest2->frequency);
            parent->left = smallest1;
            parent->right = smallest2;

            heap[heapSize] = parent;
            heapSize++;

            sortHeap(heap, heapSize);
        }

        return heap[0];
    }

    void generateCodes(Node* root, string currentCode, string* codeTable) {
        if (root == nullptr) return;

        if (root->left == nullptr && root->right == nullptr) {
            if (currentCode == "") currentCode = "0";
            codeTable[root->character] = currentCode;
        }

        generateCodes(root->left, currentCode + "0", codeTable);
        generateCodes(root->right, currentCode + "1", codeTable);
    }

    // Memory Leak Fix
    void deleteTree(Node* root) {
        if (root == nullptr) return;
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }

private:
    void sortHeap(Node** heap, int size) {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (heap[j]->frequency < heap[j + 1]->frequency) {
                    Node* temp = heap[j];
                    heap[j] = heap[j + 1];
                    heap[j + 1] = temp;
                }
            }
        }
    }
};

// 4. MAIN FEATURES


class Compressor {
public:
    void compressFile(string inputPath, string outputPath) {
        UI::setConsoleColor(14);
        cout << "   [Status] Reading file into memory...\n";
        UI::setConsoleColor(7);

        auto start = chrono::high_resolution_clock::now();

        ifstream inFile(inputPath, ios::binary | ios::ate);
        long long fileSize = inFile.tellg();
        inFile.seekg(0, ios::beg);

        unsigned char* fileData = new unsigned char[fileSize];
        inFile.read((char*)fileData, fileSize);
        inFile.close();

        // Count Frequencies
        int freq[256] = { 0 };
        for (long long i = 0; i < fileSize; i++) {
            freq[fileData[i]]++;
        }

        // Build Tree
        HuffmanEngine huff;
        Node* root = huff.buildTree(freq);

        string codes[256];
        huff.generateCodes(root, "", codes);

        // Calculate size
        long long totalBits = 0;
        for (int i = 0; i < 256; i++) {
            if (freq[i] > 0) {
                totalBits += (long long)freq[i] * codes[i].length();
            }
        }

        long long compressedByteSize = (totalBits + 7) / 8;
        unsigned char* compressedData = new unsigned char[compressedByteSize];

        for (long long i = 0; i < compressedByteSize; i++) compressedData[i] = 0;

        // --- PACKING BITS WITH PROGRESS BAR ---
        long long currentBitIndex = 0;
        cout << "\n"; // Space for bar
        for (long long i = 0; i < fileSize; i++) {

            UI::showProgress(i, fileSize);

            string code = codes[fileData[i]];
            for (char bit : code) {
                if (bit == '1') {
                    long long bytePos = currentBitIndex / 8;
                    int bitPos = 7 - (currentBitIndex % 8);
                    compressedData[bytePos] |= (1 << bitPos);
                }
                currentBitIndex++;
            }
        }
        UI::showProgress(fileSize, fileSize);

        // Write File
        ofstream outFile(outputPath, ios::binary);
        outFile.write("UZIP", 4);
        outFile.write((char*)&fileSize, sizeof(long long));
        outFile.write((char*)&totalBits, sizeof(long long));
        outFile.write((char*)freq, sizeof(freq));
        outFile.write((char*)compressedData, compressedByteSize);
        outFile.close();

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        // Show Pro Stats
        UI::printStats(fileSize, compressedByteSize, elapsed.count());

        // Cleanup
        huff.deleteTree(root);
        delete[] fileData;
        delete[] compressedData;
    }
};

class Decompressor {
public:
    void decompressFile(string inputPath, string outputPath) {
        UI::setConsoleColor(14);
        cout << "   [Status] Reading Archive...\n";
        UI::setConsoleColor(7);

        auto start = chrono::high_resolution_clock::now();

        ifstream inFile(inputPath, ios::binary);
        if (!inFile) return;

        char magic[5] = { 0 };
        inFile.read(magic, 4);
        if (string(magic) != "UZIP") {
            UI::setConsoleColor(12);
            cout << "\n   [Error] Not a valid UZIP file.\n";
            return;
        }

        long long originalSize, totalBits;
        inFile.read((char*)&originalSize, sizeof(long long));
        inFile.read((char*)&totalBits, sizeof(long long));

        int freq[256];
        inFile.read((char*)freq, sizeof(freq));

        long long compressedSize = (totalBits + 7) / 8;
        unsigned char* compressedData = new unsigned char[compressedSize];
        inFile.read((char*)compressedData, compressedSize);
        inFile.close();

        HuffmanEngine huff;
        Node* root = huff.buildTree(freq);

        unsigned char* decodedData = new unsigned char[originalSize];
        long long writeIndex = 0;
        Node* current = root;

        // --- DECODING WITH PROGRESS BAR ---
        cout << "\n";
        for (long long i = 0; i < totalBits; i++) {

            UI::showProgress(i, totalBits); // Update Bar

            long long bytePos = i / 8;
            int bitPos = 7 - (i % 8);

            bool isOne = (compressedData[bytePos] >> bitPos) & 1;

            if (isOne) current = current->right;
            else       current = current->left;

            if (current->left == nullptr && current->right == nullptr) {

                decodedData[writeIndex] = current->character;
                writeIndex++;
                current = root;
            }
        }
        UI::showProgress(totalBits, totalBits);

        ofstream outFile(outputPath, ios::binary);
        outFile.write((char*)decodedData, originalSize);
        outFile.close();

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        // Stats (Compressed size is roughly the read buffer size)
        UI::printStats(originalSize, compressedSize, elapsed.count());

        huff.deleteTree(root);
        delete[] compressedData;
        delete[] decodedData;
    }
};


// 5. MAIN MENU


int main() {
    // Set Code Page to support Box Characters (Standard Windows)
    SetConsoleOutputCP(437);

    Compressor comp;
    Decompressor decomp;

    while (true) {
        UI::printHeader("MAIN MENU");

        cout << "   [1] Compress File\n";
        cout << "   [2] Decompress File\n";
        cout << "   [3] Exit\n\n";

        UI::setConsoleColor(14);
        cout << "   Select Option: ";
        UI::setConsoleColor(7);

        string input;
        getline(cin, input);

        if (input == "3") break;

        if (input == "1") {
            UI::printHeader("COMPRESS MODE");
            string inPath = UI::getValidPath();

            UI::setConsoleColor(14);
            cout << "   >> Output Filename: ";
            UI::setConsoleColor(7);

            string outPath;
            getline(cin, outPath);

            comp.compressFile(inPath, outPath);

            cout << "\n   Press Enter to return...";
            getchar(); // Pause to see stats
        }
        else if (input == "2") {
            UI::printHeader("DECOMPRESS MODE");
            string inPath = UI::getValidPath();

            UI::setConsoleColor(14);
            cout << "   >> Output Filename: ";
            UI::setConsoleColor(7);

            string outPath;
            getline(cin, outPath);

            decomp.decompressFile(inPath, outPath);

            cout << "\n   Press Enter to return...";
            getchar();
        }
    }
    return 0;
}

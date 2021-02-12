#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include <cmath>

/**
 * Restricts color value to range [0, 255]
 */
short bound(short colorValue) {
    return std::max(std::min(colorValue, (short)255), (short)0);
}

/**
 * Writes RGB color data in PPM format to a file.
 */
template <size_t rows, size_t cols>
void write(std::tuple<short, short, short> (&colorData)[rows][cols], std::ostream& out) {
    out << "P3\n";
    out << rows << " " << cols << "\n";
    out << 255 << "\n"; 

    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            short color, green, blue;
            std::tie(color, green, blue) = colorData[i][j];
            out << color << " " << green << " " << blue << (j == cols - 1 ? "\n" : " ");
        }
    }
}

/**
 * Fills color data with black
 */
template<size_t rows, size_t cols>
void clear(std::tuple<short, short, short> (&colorData)[rows][cols]) {
    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            colorData[i][j] = {0, 0, 0};
        }
    }
}

/**
 * Draws a line from point a to point b
 */
template<size_t rows, size_t cols>
void drawLine(std::tuple<short, short, short> (&colorData)[rows][cols], std::pair<int, int> a, std::pair<int, int> b, std::tuple<short, short, short> color) {
    int dx = (b.first - a.first), dy = (b.second - a.second);
    
    // Gentler slopes
    if(abs(dx) >= abs(dy)) {
        
        int max_x, x, y;
        if(a.first <= b.first) {
            std::tie(x, y) = a;
            max_x = b.first;
        } else {
            std::tie(x, y) = b;
            max_x = a.first;
            dx = -1 * dx;
            dy = -1 * dy;
        }

        // Octant 1 and 5
        if(dy >= 0) {
            int midCompare = 2 * dy - dx; // 2A + B
            while(x <= max_x) {
                colorData[y][x] = color;
                if(midCompare >= 0) {
                    ++y;
                    midCompare -= 2 * dx;
                }
                midCompare += 2 * dy;
                ++x;
            }
        // Octant 4 and 8
        } else {
            int midCompare = 2 * dy + dx; // 2A - B
            while(x <= max_x) {
                colorData[y][x] = color;
                if(midCompare <= 0) {
                    --y;
                    midCompare += 2 * dx;
                }
                midCompare += 2 * dy;
                ++x;
            }
        }
    
    // Steeper slopes
    } else {

        int max_y, x, y;
        if(a.second <= b.second) {
            std::tie(x, y) = a;
            max_y = b.second;
        } else {
            std::tie(x, y) = b;
            max_y = a.second;
            dx = -1 * dx;
            dy = -1 * dy;
        }

        // Octant 2 and 6
        if(dx >= 0) {
            int midCompare = dy - 2 * dx; // A + 2B
            while(y <= max_y) {
                colorData[y][x] = color;
                if(midCompare <= 0) {
                    ++x;
                    midCompare += 2 * dy;
                }
                midCompare -= 2 * dx;
                ++y;
            }
        // Octant 3 and 7
        } else {
            int midCompare = dy + 2 * dx; // A - 2B
            while(y <= max_y) {
                colorData[y][x] = color;
                if(midCompare >= 0) {
                    --x;
                    midCompare -= 2 * dy;
                }
                midCompare -= 2 * dx;
                ++y;
            }
        }
    }

}

int main() {
    std::ofstream fout("bresenham.ppm");
    const int XRES = 512;
    const int YRES = 512;
    std::tuple<short, short, short> color = {0, 255, 0};

    std::tuple<short, short, short> image[XRES][YRES];
    
    clear(image);
    //octants 1 and 5 - green
    drawLine(image, {0, 0}, {XRES-1, YRES-1}, color);
    drawLine(image, {0, 0}, {XRES-1, YRES / 2}, color);
    drawLine(image, {XRES-1, YRES-1}, {0, YRES / 2}, color);

    //octants 8 and 4 - cyan
    color = {0, 255, 255};
    drawLine(image, {0, YRES-1}, {XRES-1, 0}, color);
    drawLine(image, {0, YRES-1}, {XRES-1, YRES/2}, color);
    drawLine(image, {XRES-1, 0}, {0, YRES/2}, color);

    //octants 2 and 6 - red
    color = {255, 0, 0};
    drawLine(image, {0, 0}, {XRES/2, YRES-1}, color);
    drawLine(image, {XRES-1, YRES-1}, {XRES/2, 0}, color);

    //octants 7 and 3 - magenta
    color = {255, 0, 255};
    drawLine(image, {0, YRES-1}, {XRES/2, 0}, color);
    drawLine(image, {XRES-1, 0}, {XRES/2, YRES-1}, color);

    //horizontal and vertical - yellow
    color = {255, 255, 0};
    drawLine(image, {0, YRES/2}, {XRES-1, YRES/2}, color);
    drawLine(image, {XRES/2, 0}, {XRES/2, YRES-1}, color);

    write(image, fout);
    fout.close();
}
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
            short color, bezierColor, blue;
            std::tie(color, bezierColor, blue) = colorData[i][j];
            out << color << " " << bezierColor << " " << blue << (j == cols - 1 ? "\n" : " ");
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

/**
 * Translates every point (x, y) up sinx.
 * More specifically, sets all (x, y) to the color of (x, y - sinx) - if out of bounds, default black.
 */
template<size_t rows, size_t cols>
void drawQuadraticBeziers(std::tuple<short, short, short> (&colorData)[rows][cols], std::tuple<short, short, short> bezierColor, std::tuple<short, short, short> diagonalColor){
    for(int i = 0; i < 64; ++i) {
        // Bezier curves
        drawLine(colorData, {i * (cols - 1) / 64, 0}, {0, (64 - i) * (rows - 1) / 64}, bezierColor);
        drawLine(colorData, {i * (cols - 1) / 64, rows - 1}, {0, i * (rows - 1) / 64}, bezierColor);
        drawLine(colorData, {i * (cols - 1) / 64, 0}, {cols - 1, i * (rows - 1) / 64}, bezierColor);
        drawLine(colorData, {i * (cols - 1) / 64, rows - 1}, {cols - 1, (64 - i) * (rows - 1) / 64}, bezierColor);
        // Diagonal lines
        drawLine(colorData, {i * (cols - 1) / 64, 0}, {0, i * (rows - 1) / 64}, diagonalColor);
        drawLine(colorData, {i * (cols - 1) / 64, rows - 1}, {cols - 1, i * (rows - 1) / 64}, diagonalColor);
        drawLine(colorData, {(64 - i) * (cols - 1) / 64, 0}, {cols - 1, i * (rows - 1) / 64}, diagonalColor);
        drawLine(colorData, {(64 - i) * (cols - 1) / 64, rows - 1}, {0, i * (rows - 1) / 64}, diagonalColor);
    }
}

int main() {
    std::ofstream fout("bresenham.ppm");
    const int XRES = 512;
    const int YRES = 512;
    std::tuple<short, short, short> color = {0, 255, 0};
    const std::tuple<short, short, short> BEZIER_COLOR = {165, 0, 0};
    const std::tuple<short, short, short> DIAGONAL_COLOR = {80, 0, 0}; 
    std::tuple<short, short, short> image[XRES][YRES];
    
    clear(image);

    // drawQuadraticBeziers(image, BEZIER_COLOR, DIAGONAL_COLOR);

    //octants 1 and 5 - bezierColor
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
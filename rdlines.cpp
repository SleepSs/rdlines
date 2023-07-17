#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <termios.h>

class Readline {
public:
    Readline() {
        // 获取终端属性
        tcgetattr(STDIN_FILENO, &orig_termios_);
        raw_termios_ = orig_termios_;
        // 禁用回显和规范模式
        raw_termios_.c_lflag &= ~(ECHO | ICANON);
        // 设置最小字符数和等待时间
        raw_termios_.c_cc[VMIN] = 1;
        raw_termios_.c_cc[VTIME] = 0;
        // 应用新的终端属性
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios_);
    }

    ~Readline() {
        // 恢复终端属性
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_);
    }

    std::string readline() {
        std::string line;
        std::string ansi_seq;
        bool in_ansi_seq = false;
        size_t cursor_pos = 0;

        while (true) {
            // 读取一个字符
            char c;
            read(STDIN_FILENO, &c, 1);

            if (in_ansi_seq) {
                // 在 ANSI 转义序列中
                ansi_seq += c;
                if (c >= '\x40' && c <= '\x7e') {
                    // 转义序列结束
                    in_ansi_seq = false;
                    if (ansi_seq == "\x1b[A") {
                        // 上箭头键，历史记录
                    } else if (ansi_seq == "\x1b[B") {
                        // 下箭头键，历史记录
                    } else if (ansi_seq == "\x1b[C") {
                        // 右箭头键，向右移动光标
                        if (cursor_pos < line.size()) {
                            cursor_pos++;
                        }
                    } else if (ansi_seq == "\x1b[D") {
                        // 左箭头键，向左移动光标
                        if (cursor_pos > 0) {
                            cursor_pos--;
                        }
                    }
                }
            } else if (c == '\n') {
                // 回车键，返回输入的行
                std::cout << std::endl;
                return line;
            } else if (c == '\x1b') {
                // 转义键，进入 ANSI 转义序列
                in_ansi_seq = true;
                ansi_seq += c;
            } else if (c == '\x7f') {
                // 退格键，删除前一个字符
                if (cursor_pos > 0) {
                    line.erase(cursor_pos - 1, 1);
                    cursor_pos--;
                }
            } else {
                // 插入一个字符
                line.insert(cursor_pos, 1, c);
                cursor_pos++;
            }

            // 清空行并重新打印
            std::cout << "\r\033[K";
            size_t i = 0;
            while (i < line.size()) {
                if (line[i] == '\x1b') {
                    // 解析 ANSI 转义序列
                    size_t j = i + 1;
                    while (j < line.size() && line[j] >= '\x40' && line[j] <= '\x7e') {
                        j++;
                    }
                    if (j > i + 1) {
                        std::string ansi_seq = line.substr(i, j - i + 1);
                        std::cout << ansi_seq;
                        i = j + 1;
                        continue;
                    }
                }
                if (i == cursor_pos - 1) {
                    std::cout << "\033[31m" << line[i] << "\033[0m";
                } else {
                    std::cout << line[i];
                }
                i++;
            }
            // 将光标移动到正确的位置
            std::cout << "\r";
            for (size_t i = 0; i < cursor_pos; i++) {
                if (line[i] == '\x1b') {
                    // 跳过 ANSI 转义序列
                    while (i < line.size() && line[i] >= '\x40' && line[i] <= '\x7e') {
                        i++;
                    }
                }
                std::cout << "\033[1C";
            }
        }
    }

private:
    struct termios orig_termios_;
    struct termios raw_termios_;
};

int main() {
    Readline rl;
    std::string line = rl.readline();
    std::cout << "You entered: " << line << std::endl;
    return 0;
}
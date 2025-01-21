#include <random>             // for random number generation
#include <iostream>           // for I/O streams
#include <iomanip>            // for setfill & setw
#include <thread>             // for concurrency (threading)
#include <atomic>             // for atomic variables
#include <format>             // for formatted output
#include <syncstream>         // for synchronized output stream (std::osyncstream)


class Guess {
public:
    void run() {
        char userChoices{};
        int guess{}, random{};
        std::atomic<bool> runFlag{ true }, first{ true };

        while (true) {
            clearConsole();

            std::cout << std::format("{}{}Guessing Game!\n{}", getANSI(ANSI::BackgroundWhite), getANSI(ANSI::TextBlack), getANSI(ANSI::Reset));

            std::thread generator([&]() -> void {
                do {
                    std::osyncstream out(std::cout);
                    random = m_1to100_R(seed);
                    if (not first)
                        out << "\033[s\r";  // save current cursor position

                    out << "Random number -> [" << std::setfill('0') << std::setw(3) << random << "] guess the number from (1 to 100): ";

                    if (not first)
                        out << "\033[u"; // restore current cursor position
                    else
                        first = false;
                } while (runFlag);
                });

            std::cin >> guess;
            runFlag = false;
            generator.join(); //  waiting for the generator to finish its job.
            clearConsole();

            std::cout << std::format("{}{}Guessing Game!\n{}", getANSI(ANSI::BackgroundWhite), getANSI(ANSI::TextBlack), getANSI(ANSI::Reset));
            std::cout << "Random number -> [" << std::setfill('0') << std::setw(3) << random << "] guess the number from (1 to 100): " << guess << '\n';

            if (random == guess)
                std::cout << "Congrats! You guessed [ " << guess << " <=> " << random << " ] correctly!\n";
            else if (guess <= 0)
                std::cout << "Unfortunately... you got it wrong. The number you entered: " << guess << " is too low.\n";
            else if (guess > 100)
                std::cout << "Unfortunately... you got it wrong. The number you entered: " << guess << " is too high.\n";
            else
                std::cout << "Unfortunately... you got it wrong. The number you entered: " << guess << " isn't " << random << ".\n";

            /* clearing the input stream */
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.clear();

            std::cout << "Would you like to try again (y/n): ";

            while (true) {
                std::cin >> userChoices;
                if (userChoices == 'y') {
                    runFlag = true;
                    first = true;
                    break;
                }
                else if (userChoices == 'n') {
                    std::printf("%s", getANSI(ANSI::Reset));
                    clearConsole();
                    return;
                }
                else
                    std::cout << "Enter only y or n: ";
            }
        }
    }



    enum class ANSI {
        ClearScreen,
        ResetCursor,
        BackgroundWhite,
        TextBlack,
        Reset
    };

    /* static because there is no reason for getANSI to be an instance function */
    static const char* getANSI(enum ANSI code) {
        switch (code) {
        case ANSI::ClearScreen:     return "\033[2J";
        case ANSI::ResetCursor:     return "\033[H";
        case ANSI::BackgroundWhite: return "\033[48;2;255;255;255m";
        case ANSI::TextBlack:       return "\033[38;2;0;0;0m";
        case ANSI::Reset:           return "\033[0m";
        default:                    return "";
        }
    }

    /* same thing here */
    static std::string setColor(const int r, const int g, const int b) {
        return std::format("\033[38;2;{};{};{}m", r, g, b);
    }

    static void clearConsole() {
        /* if it detect you running this program windows... it will execute the `std::system` to clear the screen
        , otherwise it uses the ansi escape code to clear the screen (for UNIX-like {FreeBSD})*/
        #ifdef _WIN32
        std::system("cls");
        #else
        std::cout << getANSI(ANSI::ClearScreen) << getANSI(ANSI::ResetCursor) << std::flush;
        #endif
    }

private:
    std::random_device rd;
    std::mt19937 seed{ rd() };      // used for seeding random numbers
    std::uniform_int_distribution<> m_1to100_R{ 1, 100 };  // the reason for <> is because the class is instantiatly for the int type by default
};

int main() {
    class Guess guess;  // The class from the beginning is useless, but I just like the declaration to resemble a C struct (without typedef). (Just my opinion)
    guess.run();    // invoking the run method.
    std::cout << std::format("{}bye!{}\n", Guess::setColor(0, 255, 0), Guess::getANSI(Guess::ANSI::Reset));
    return EXIT_SUCCESS;
}

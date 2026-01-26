#include <iostream>
#include <chrono>
#include <thread>

struct Converter{
    void toBinary(unsigned int decimal, int bits = 10){
        if(bits >=  0 && bits <= 10){
            for(int i = bits-1; i >= 0; i--){
                std::cout << ((decimal >> i) & 1);
            }
            std::cout << std::endl;
        }else{
            std::cerr << "error :: reset Qpin isn't correct. Please, put a pin between 0 and 10" << std::endl;
        }
    }
};


class Chip4017{
    private:
        unsigned int Out = 1;
        unsigned int LimitReset;

    public:
        Chip4017(double limitReset) : LimitReset(limitReset) {};

        void shift(){
            if(LimitReset <= 10){ //comparison of unsigned expression in ‘>= 0’ is always true
                if(Out >= (1u << (LimitReset-1))){
                    reset();
                } else {
                    Out = Out << 1;
                }
            }
        }

        void reset(){
            Out = 1;
        }

        unsigned int getOut() const{
            return Out;
        }
};


class Chip555{
    private:
        bool output = false;
        double R1, R2, C;
        const double Ln2 = 0.693;

    public:
        Chip555(double r1, double r2, double c ) 
            : R1(r1), R2(r2), C(c) {};

        double tHigh(){
            return Ln2*(R1 + R2)*C;
        }

        double tLow(){
            return Ln2*R2*C;
        }

        bool clock(){
            output = !output;
            return output;
        }

        void pulse(){
            clock();
            std::this_thread::sleep_for(std::chrono::duration<double>(tHigh()));
            clock();
            std::this_thread::sleep_for(std::chrono::duration<double>(tLow()));
        }
};


int main(){ 
    const int resetQP = 10;

    Chip4017 chip4017(resetQP);
    Chip555 chip555(1000, 10000, 7.37e-6);
    Converter converter;

    // cycle 
    while(true){
        converter.toBinary(chip4017.getOut(), resetQP);
        chip555.pulse();
        chip4017.shift();
    }

    return 0;
}
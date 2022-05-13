

template<int N,int M>
class Matrix{
    int tab[N][M];

public:
    Matrix() {
    }
};

class Complex{

    int r; // reel
    int i; // imaginaire

public :

    Complex(int _r,int _i) :r(_r),i(_i) {}

    int GetReel(){
        return this->r;
    }

    int GetImaginary(){
        return this->i;
    }

    int GetReel() const {
        return this->r;
    }

    const int GetImaginary() const{
        return this->i;
    }

    Complex operator+=(const Complex _c){
        Complex c(this->r + _c.GetReel(),this->i + _c.GetImaginary());
        return c;
    }

    Complex operator+(const Complex _c){
        return *this+=_c;
    }

    Complex operator-=(const Complex _c){
        Complex c(this->r - _c.GetReel(),this->i - _c.GetImaginary());
        return c;
    }

    Complex operator-(const Complex _c){
        return *this-=_c;
    }


    Complex operator*=(const Complex _c){
        Complex c(this->r * _c.GetReel() - this->r * _c.GetReel(),i*_c.GetReel() + r* _c.GetImaginary() );
        return c;
    }

    Complex operator*(const Complex _c){
        return *this*=_c;
    }


};

std::ostream& operator<<(std::ostream& os, Complex _c){
    return os << _c.GetReel() << " | "<<_c.GetImaginary() << std::endl;
}


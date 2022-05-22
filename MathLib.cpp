template<int N,int M>

class Matrix{
    float tab[N*M];

public:
    Matrix(float t[N*M] ) {

        /* A CHANGER */
        for (int i = 0; i < N ; ++i) {
            for (int j = 0; j < M; ++j) {
                tab[(i*N)+j] = t[(i*N)+j];
                // std::cout << tab[i][j] << " | i : " << i << " j  :"<< j;
            }
            //  std::cout << std::endl;
        }
    }

    Matrix(int t[N*M] ) {

        /* A CHANGER */
        for (int i = 0; i < N ; ++i) {
            for (int j = 0; j < M; ++j) {
                tab[(i*N)+j] = t[(i*N)+j];
            }
        }
    }

    Matrix(){
        for (int i = 0; i < N ; ++i) {
            for (int j = 0; j < M; ++j) {
                tab[i*j] = 0;
                if(i == j){
                    tab[(i*N)+ i] = 1;
                }
            }
        }
    };

    float* getTab(){
        return this->tab;
    }

    void setAt(int i,int j,int v){
        tab[(i*N)+j] = v;
    }

    void display (){
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                std::cout<<tab[i*N + j] << " ";
            }
            std::cout  << std::endl;
        }

    }

    float operator()(int i,int j ){
        return tab[(i*N)+j];
    }

};


class Quaternion{
    float tab[4]{};

    public:
    Quaternion(float x,float y,float z,float w){
        tab[0] = x;
        tab[1] = y;
        tab[2] = z;
        tab[3] = w;
    }

    Quaternion(float axe[3],float angle){

        this->setAngle_Axis(axe,angle);


    }

    explicit Quaternion(const float _f[4]){
        for (int i = 0; i < (int)sizeof(tab)/sizeof(int); ++i) {
            tab[i] = _f[i];
        }
    }

    void setAngle_Axis(const float *axe, float angle){
        angle = (angle/2) * 3.14159 / 180.0;

        tab[0] = axe[0] * sinf(angle);
        tab[1] = axe[1] * sinf(angle);
        tab[2] =  axe[2]* sinf(angle);
        tab[3] = cosf(angle);

    }

    float* getTab(){
        return tab;
    }

    float getNorm(){
        return sqrtf(powf(tab[0],2) + powf(tab[1],2) + powf(tab[2],2)  + powf(tab[3],2)  );
    }

    float* getQutrnormalized(){
        float norm = this->getNorm();

        float  *tmp = new float[4] {tab[0] / norm, tab[1] / norm, tab[2] / norm, tab[3] / norm };

        return tmp;
    }


    float* conj(){
        static float tmp[4] {-this->tab[0],-this->tab[1],-this->tab[2],this->tab[3]};
        return tmp;
    }


    float dot(Quaternion qtr){
        return (this->tab[0]*qtr.tab[0]) + (this->tab[1]*qtr.tab[1]) + (this->tab[2]*qtr.tab[2]) + (this->tab[3]*qtr.tab[3]);
    }

    float* CrossProduct(Quaternion qtr){
        static float t [3] {
            this->tab[1]*qtr.tab[2] -  this->tab[2]*qtr.tab[1],
            this->tab[2]*qtr.tab[0] -  this->tab[0]*qtr.tab[2],
            this->tab[0]*qtr.tab[1] -  this->tab[1]*qtr.tab[0]
        };

        return t;
    }


    Matrix<4,4> getRotationMatrix(){

        Quaternion q(this->getQutrnormalized());

        std::cout << q.tab[0] <<" " << q.tab[1] <<" " << q.tab[2] <<" " << q.tab[3]   << std::endl;

        float a = q.tab[3];
        float b = q.tab[0];
        float c = q.tab[1];
        float d = q.tab[2];

        float tmp[4*4] {
                1-(2*powf(c,2)) - 2*powf(d,2), 2*b*c + 2*d*a , 2*b*d - 2*c*a,0,
                2*b*c - 2*d*a, 1 - 2*powf(b,2) - 2*powf(d,2), 2*c*d + 2*b*a,0,
                2*b*d + 2*c*a, 2*c*d - 2*b*a , 1 - 2*powf(b,2) - 2*powf(c,2),0,
                0.0,0.0,0,1.0f
        };

        Matrix<4,4> m(tmp);

        return  m;
    }


    Matrix<4,4> getMatrix(){

        float f[4*4] {
             this->tab[3], this->tab[0], this->tab[1], this->tab[2],
            -this->tab[0], this->tab[3], this->tab[2],-this->tab[1],
            -this->tab[1],-this->tab[2], this->tab[3], this->tab[0],
            -this->tab[2], this->tab[1],-this->tab[0], this->tab[3]
        };



        Matrix<4,4>m(f);

        return m;
    }


    Quaternion operator*=(Quaternion qtr){
        float q[4];

        q[3] = (this->tab[3]*qtr.tab[3]) - (this->tab[0]*qtr.tab[0]) - (this->tab[1]*qtr.tab[1]) - (this->tab[2] * qtr.tab[2]);
        q[0] = (this->tab[3] * qtr.tab[0]) + (this->tab[0]*qtr.tab[3]) + (this->tab[1] * qtr.tab[2]) - (this->tab[2] * qtr.tab[1]);
        q[1] = (this->tab[3]*qtr.tab[1]) - (this->tab[0] * qtr.tab[2]) + (this->tab[1]*qtr.tab[3]) + (this->tab[2] * qtr.tab[0]);
        q[2] = (this->tab[3]*qtr.tab[2]) + (this->tab[0] * qtr.tab[1]) - (this->tab[1]*qtr.tab[0]) + (this->tab[2] * qtr.tab[3]);

        return Quaternion(q);
    }

    Quaternion operator*(Quaternion qtr){
        return *this*=qtr;
    }


    Quaternion operator+=(Quaternion qtr){

        return {this->tab[0] + qtr.tab[0],this->tab[1] + qtr.tab[1],this->tab[2] + qtr.tab[2],this->tab[3] + qtr.tab[3]};
    }

    Quaternion operator+(Quaternion qtr){
        return *this+=qtr;
    }

    Quaternion operator-=(Quaternion qtr){

        return {this->tab[0] - qtr.tab[0],this->tab[1] - qtr.tab[1],this->tab[2] - qtr.tab[2],this->tab[3] - qtr.tab[3]};
    }

    Quaternion operator-(Quaternion qtr){
        return *this-=qtr;
    }


};






template<int N, int M,int O,int P>
Matrix<N,P> multMatrice(Matrix<N,M> m1,Matrix<O,P> m2){
    Matrix<N,P> m;
    m.display();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            m.setAt(i,j,0);

            for (int k = 0; k < P; k++) {
                m.setAt(i,j, m(i,j) + m1(i,k) * m2(k,j));
            }
        }
    }
    return m;
}



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

template<int N,int M>
std::ostream& operator<<(std::ostream& os, Matrix<N,M> _m){


    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            os<< _m(i,j) << " ";
        }
        os  << std::endl;
    }

    return os;
}


std::ostream& operator<<(std::ostream& os, Quaternion q){
    float * tab = &q.getTab()[0];

    return os << "x : " << tab[0] << " y : " << tab[1] << " z : " << tab[2] << " w : " <<tab[3] << std::endl;
}



class Camera{

    public:

    float deltaForward;
    float deltaStrafe;

    float posx;
    float posy;
    float posz;

    float dirx ;
    float diry ;
    float dirz ;

    float angleh ;
    float anglev ;

    float xOrigin ;
    float yOrigin ;

    float deltaAnglex ;
    float deltaAngley ;

    int locked;

    float SENSIBILITY = 0.5f;


    Camera();
    void orienterCam(int x, int y);

};

Camera::Camera()
{
    deltaForward = 0;
    deltaStrafe = 0;

    posx = 0.0f;
    posy = 1.5f;
    posz = 5.0f;

    dirx = 0.0f;
    diry = 0.0f;
    dirz = -1.0f;

    angleh = 0.0f;
    anglev = 0.0f;

    xOrigin = -1;
    yOrigin = -1;

    deltaAnglex = 0.0f;
    deltaAngley = 0.0f;



    locked = 1;
}

void Camera::orienterCam(int x, int y)
{

        // Calcul de l'angle horizontal
        deltaAnglex = (x - xOrigin) * SENSIBILITY;
        // Correction de l'angle ]-Pi; Pi[
        while (deltaAnglex + angleh > M_PI)
            deltaAnglex -= M_PI * 2;
        while (deltaAnglex + angleh < -M_PI)
            deltaAnglex += M_PI * 2;

        // Calcul de l'angle vertical
        deltaAngley = (y - yOrigin) * SENSIBILITY;
        // Limitation de l'angle (limite haute)
        if (deltaAngley + anglev > M_PI_2)
            deltaAngley = M_PI_2 - anglev - 0.01f;
        // Limitation de l'angle (limite basse)
        if (deltaAngley + anglev < -M_PI_2)
            deltaAngley = -M_PI_2 - anglev + 0.01f;

        // Mis � jour de la cam�ra
        dirx = sinf(angleh + deltaAnglex)*cosf(anglev + deltaAngley);
        diry = -sinf(anglev + deltaAngley);
        dirz = -cosf(angleh + deltaAnglex)*cosf(anglev + deltaAngley);

}

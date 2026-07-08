// PieSensor.cpp : implementation file
//

#include "stdafx.h"
#include "PieSensor.h"

#include <math.h>

#define RADIUS(x) x * 3.1415 / 180
#define FI 7.5

// ²¶×½Êó±êÎ»ÖÃ
const double dpTan[13] = {0, -0.13, -0.4, -0.8, -1.3, -2.4, -8, 8, 2.5, 1.3, 0.8, 0.4, 0.13};
const double dpOffset[13] = {0, 0.13, 0.2, 0.23, 0.45, 1.2, 30, 30, 1.2, 0.45, 0.23, 0.2, 0.13};

double inline ABS(double x)
{
    if (x <= 0)
        x = -x;
    return x;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPieSensor

CPieSensor::CPieSensor()
{
    m_pInfrared = NULL;
    m_pUltrasonicSensor = NULL;
    m_pbSonicEnable = NULL;

    m_pInfraredX = NULL;
    m_pInfraredY = NULL;

    m_nInfrared = 0;
    m_nUltrasonicSensor = 0;

    m_uBkPicID = 0;
    m_uShelterID = 0;

    m_nCenterX = 0;
    m_nCenterY = 0;

    m_scale = 54.0f;
    m_nSelectSensor = 0;
    m_dDistance = 0;

    InitSensor();
}

CPieSensor::~CPieSensor()
{
    ReleaseInfrared();
    ReleaseUltrasonic();
}

BEGIN_MESSAGE_MAP(CPieSensor, CStatic)
//{{AFX_MSG_MAP(CPieSensor)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPieSensor message handlers

void CPieSensor::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // TODO: Add your message handler code here
    CDC dcMemory;
    dcMemory.CreateCompatibleDC(&dc);

    CBitmap robotbitmap;
    CBitmap shelterbitmap;

    if (m_uBkPicID != 0)
    {
        // draw background bitmap
        robotbitmap.LoadBitmap(m_uBkPicID);

        dcMemory.SelectObject(&robotbitmap);

        CPen ultrasonicpen(PS_SOLID, 2, COLORREF(RGB(0, 0, 255)));

        CPen *pOldPen = dcMemory.SelectObject(&ultrasonicpen);

        bool bfirstenable = false;
        int firstindex = 0;

        for (int i = 0; i <= m_nUltrasonicSensor; i++)
        {
            if (m_pbSonicEnable[i] && !bfirstenable)
            {
                firstindex = i;
                dcMemory.MoveTo(CalPoint(i, i));
                bfirstenable = true;
                continue;
            }
            if (m_pbSonicEnable[i] && bfirstenable)
                dcMemory.LineTo(CalPoint(i, firstindex));
        }

        dcMemory.LineTo(CalPoint(firstindex, firstindex));

        dcMemory.SelectObject(&pOldPen);
    }

    if (m_uShelterID != 0)
    {
        // draw shelter, means infrared sensor information
        CDC dcInfraRed;
        dcInfraRed.CreateCompatibleDC(&dc);
        shelterbitmap.LoadBitmap(m_uShelterID);
        dcInfraRed.SelectObject(&shelterbitmap);

        for (int i = 0; i < m_nInfrared; i++)
        {
            if (m_pInfrared[i] && m_pInfrared != NULL)
                dcMemory.BitBlt(m_pInfraredX[i] - m_sheltersize.width / 2, m_pInfraredY[i] - m_sheltersize.height / 2,
                                m_sheltersize.width, m_sheltersize.height, &dcInfraRed, 0, 0, SRCCOPY);
        }
        dcInfraRed.DeleteDC();
    }

    dc.BitBlt(0, 0, m_bksize.width, m_bksize.height, &dcMemory, 0, 0, SRCCOPY);
    dcMemory.DeleteDC();

    // Do not call CStatic::OnPaint() for painting messages
}

// set background bitmap and shelter bitmap for background and infrared sensors
void CPieSensor::SetBitmap(UINT picid, UINT shelterid)
{
    // Set background bitmap resource ID and shelter resource ID
    m_uBkPicID = picid;
    m_uShelterID = shelterid;
}

// initialize information of all sensors, including ultrasonic sensors and infrared sensors
// infrared is the total amount of infrared sensors
// and ultrasonicsensor is the total amount of ultrasonic sensors
void CPieSensor::InitSensor()
{
    // initialize sensor amount information
    ReleaseInfrared();
    ReleaseUltrasonic();

    m_nInfrared = SENSORNUM;
    m_nUltrasonicSensor = SENSORNUM;

    m_pUltrasonicSensor = new double[SENSORNUM];
    m_pInfrared = new bool[SENSORNUM];
    m_pInfraredX = new int[SENSORNUM];
    m_pInfraredY = new int[SENSORNUM];
    m_pbSonicEnable = new bool[SENSORNUM];

    // 1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24
    int infraradx[24] = {250, 295, 336, 367, 383, 390, 385, 367, 335, 297, 250, 200,
                         150, 105, 62,  33,  12,  10,  12,  30,  62,  100, 150, 198};
    int infrarady[24] = {13,  33,  63,  103, 148, 197, 245, 290, 330, 360, 380, 388,
                         380, 365, 335, 295, 248, 200, 150, 100, 62,  30,  15,  10};

    memcpy(m_pInfraredX, infraradx, sizeof(int) * m_nInfrared);
    memcpy(m_pInfraredY, infrarady, sizeof(int) * m_nInfrared);

    for (int i = 0; i < m_nUltrasonicSensor; i++)
    {
        m_pUltrasonicSensor[i] = 0;
        m_pbSonicEnable[i] = true;
    }

    for (i = 0; i < m_nInfrared; i++)
        m_pInfrared[i] = false;

    m_nCenterX = 199;
    m_nCenterY = 198;

    m_bksize.height = 400;
    m_bksize.width = 400;

    m_sheltersize.height = 18;
    m_sheltersize.width = 20;
}

////release infrared sensor information resource
void CPieSensor::ReleaseInfrared()
{
    m_nInfrared = 0;
    if (m_pInfrared != NULL)
        delete[] m_pInfrared;

    if (m_pInfraredX != NULL)
        delete[] m_pInfraredX;

    if (m_pInfraredY != NULL)
        delete[] m_pInfraredY;

    m_pInfraredX = NULL;
    m_pInfraredY = NULL;
    m_pInfrared = NULL;
}

// release ultrasonic sensor information resource
void CPieSensor::ReleaseUltrasonic()
{
    m_nUltrasonicSensor = 0;
    if (m_pUltrasonicSensor != NULL)
        delete[] m_pUltrasonicSensor;
    m_pUltrasonicSensor = NULL;

    if (m_pbSonicEnable != NULL)
        delete[] m_pbSonicEnable;
    m_pbSonicEnable = NULL;
}

// length in every axis equals m_pUltrasonicSensor[i] by scale
// CalPoint function calculate coordinate of every ultrasonic sensor data
// each axis should be divided averagely
// each axis should be clockwise
CPoint CPieSensor::CalPoint(int iPoint, int firstindex)
{
    CPoint point(m_nCenterX, m_nCenterY);
    if (iPoint > m_nUltrasonicSensor - 1)
        iPoint = firstindex;

    double length = m_pUltrasonicSensor[iPoint] * m_scale;

    if (length > ABS(m_bksize.width - m_nCenterX))
        length = ABS(m_bksize.width - m_nCenterX);
    if (length > ABS(m_bksize.height - m_nCenterY))
        length = ABS(m_bksize.width - m_nCenterX);

    // calculate angle of ultrasonic sensor with certain index
    double angle = 360.0 / m_nUltrasonicSensor * iPoint + 15;

    // calculate coordinate of certain sensor
    double lengthsin = length * sin(RADIUS(angle));
    double lengthcos = length * cos(RADIUS(angle));

    point.x = (long)(m_nCenterX + lengthsin);
    point.y = (long)(m_nCenterY - lengthcos);

    return point;
}

// set up parameter of line of the ultrasonic sensor information
// x,y defines the centor of the map or the zero point of the reference frame
void CPieSensor::SetLine(DOUBLE scale)
{
    // m_scale=scale*2;
}

// call this function to update infrared sensors data
// then invoke invalidate function to update picture
void CPieSensor::UpdateInfrared(BOOL *pinfrared)
{
    for (int i = 0; i < m_nInfrared; i++)
        m_pInfrared[i] = pinfrared[i];
}

// call this function to update ultrasonic sensors data
// then invoke invalidate function to update picture
void CPieSensor::UpdateUntrasonic(DOUBLE *pultrasonic, BOOL *pbEnable)
{
    for (int i = 0; i < m_nUltrasonicSensor; i++)
    {
        m_pUltrasonicSensor[i] = pultrasonic[i];
        if (pbEnable != NULL)
            m_pbSonicEnable[i] = pbEnable[i];
    }
}

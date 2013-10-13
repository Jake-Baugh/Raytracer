#ifndef RAYTRACER_TIMER_H
#define RAYTRACER_TIMER_H

#include <d3d11.h>

class Timer
{
public:
	Timer();
	~Timer();

	void start(ID3D11DeviceContext* p_context);
	void stop(ID3D11DeviceContext* p_context);

	double getTime(ID3D11DeviceContext* p_context);

	HRESULT init(ID3D11Device* p_device);

private:
	ID3D11Query* m_start;
	ID3D11Query* m_stop;
	ID3D11Query* m_disjoint;
};

#endif //RAYTRACER_TIMER_H
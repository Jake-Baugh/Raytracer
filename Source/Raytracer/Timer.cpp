#include "Timer.h"
#include "Utility.h"

#include <iostream>

Timer::Timer()
{
	m_start		= nullptr;
	m_stop		= nullptr;
	m_disjoint	= nullptr;
}

Timer::~Timer()
{
	SAFE_RELEASE(m_start);
	SAFE_RELEASE(m_stop);
	SAFE_RELEASE(m_disjoint);
}

void Timer::start(ID3D11DeviceContext* p_context)
{
	p_context->Begin(m_disjoint);
	p_context->End(m_start);
}
void Timer::stop(ID3D11DeviceContext* p_context)
{
	p_context->End(m_stop);
	p_context->End(m_disjoint);
}

double Timer::getTime(ID3D11DeviceContext* p_context)
{
	UINT64 startTime = 0;
	while(p_context->GetData(m_start, &startTime, sizeof(startTime), 0) != S_OK);

	UINT64 endTime = 0;
	while(p_context->GetData(m_stop, &endTime, sizeof(endTime), 0) != S_OK);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
	while(p_context->GetData(m_disjoint, &disjointData, sizeof(disjointData), 0) != S_OK);

	double time = -1.0;
	if(disjointData.Disjoint == FALSE)
	{
		UINT64 delta = endTime - startTime;
		double frequenzy = static_cast<double>(disjointData.Frequency);
		time = (delta/frequenzy) * 1000.0f;
		if(delta > 32)
			m_times.push_back(time);
	}

	return time;
}

double Timer::getMeanTime()
{
	double meanTime = 0.0;
	for(unsigned int i=0; i<m_times.size(); i++)
		meanTime += m_times[i];

	meanTime = meanTime/m_times.size();
	return meanTime;
}

double Timer::getMax()
{
	double max = FLT_MIN;
	for(unsigned int i=0; i<m_times.size(); i++)
	{
		if(m_times[i] > max)
			max = m_times[i];
	}
	return max;
}
double Timer::getMin()
{
	double min = FLT_MAX;
	for(unsigned int i=0; i<m_times.size(); i++)
	{
		if(m_times[i] < min)
			min = m_times[i];
	}
	return min;
}

unsigned int Timer::getNumSamples()
{
	return m_times.size();
}

HRESULT Timer::init(ID3D11Device* p_device)
{
	HRESULT hr = S_OK;

	D3D11_QUERY_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	desc.MiscFlags = 0;
	hr = p_device->CreateQuery(&desc, &m_disjoint);

	desc.Query = D3D11_QUERY_TIMESTAMP;
	
	if(SUCCEEDED(hr))
		hr = p_device->CreateQuery(&desc, &m_start);
	if(SUCCEEDED(hr))
		hr = p_device->CreateQuery(&desc, &m_stop);

	return hr;
}
/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

template <typename T, int size, typename GenType>
et::Ce2Polynome<T, size, GenType>::Ce2Polynome()
{ 
	for (int i = 0; i < size; ++i)
		_coefficients[i] = T(0);
}

template <typename T, int size, typename GenType>
et::Ce2Polynome<T, size, GenType>::Ce2Polynome(const T* coefficients)
{ 
	for (int i = 0; i < size; ++i)
		_coefficients[i] = coefficients[i];
}

template <typename T, int size, typename GenType>
T et::Ce2Polynome<T, size, GenType>::calculate(GenType time) const
{
	T result = 0.0;
	T local_time = 1.0;
	for (int i = 0; i < size; ++i)
	{
		result += _coefficients[i] * local_time;
		local_time *= time;
	}
	return result;
}

template <typename T, int size, typename GenType>
T et::Ce2Polynome<T, size, GenType>::curvature(int derivativeOrder, GenType time) const
{
	T df = derivative(1 + derivativeOrder, time);
	T ddf = derivative(2 + derivativeOrder, time);
	T denominator = T(GenType(1)) + df * df;
	return abs(ddf) / (denominator * sqrt(denominator));
}

template <typename T>
inline T __intPower(T value, int power)
{
	if (power == 0) return 1;

	T result = 1.0;
	for (int i = 1; i <= power; ++i)
	{
		result *= value; 
	}
	return result;
}

template <typename T, int size, typename GenType>
T et::Ce2Polynome<T, size, GenType>::derivative(int order, GenType time) const
{
	T local_coefficients[size];
	for (int i = 0; i < size; ++i)
		local_coefficients[i] = _coefficients[i];

	for (int t = 0; t < order; ++t)
		for (int s = 0; s < size; ++s)
			local_coefficients[s] *= GenType(s - t);

	T result = 0;
	for (int i = order; i < size; ++i)
	{
		int p = i - order;
		if (p < 0) p = 0;

		T local_time = (p == 0) ? 1 : __intPower(time, p);
		result += local_coefficients[i] * local_time;
	}

	return result;
}

template <typename T, int size, typename GenType>
T et::Ce2Polynome<T, size, GenType>::numericDerivative(int order, GenType time, GenType dt)
{
	T pp = derivative(order - 1, time - dt);
	T np = derivative(order - 1, time + dt);
	return (np - pp) / (2 * dt);
}

/**
* Fully Qualified Spline
**/
template <typename T, typename GenType>
et::Ce2FullyQualifiedSpline<T, GenType>::Ce2FullyQualifiedSpline()
{
	_p0 = _dp0 = _ddp0 = T(0);
	_pT = _dpT = _ddpT = T(0);
	_time = 1;
	genPolynomial();
}

template <typename T, typename GenType>
et::Ce2FullyQualifiedSpline<T, GenType>::Ce2FullyQualifiedSpline(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE ddp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, ET_IN_TYPE ddpT, GenType time)
{
	_p0 = p0;
	_dp0 = dp0;
	_ddp0 = ddp0;
	_pT = pT;
	_dpT = dpT;
	_ddpT = ddpT;
	_time = time;
	genPolynomial();
}

template <typename T, typename GenType>
et::Ce2FullyQualifiedSpline<T, GenType>::Ce2FullyQualifiedSpline(const Ce2FullyQualifiedSpline& prev, ET_IN_TYPE pT, ET_IN_TYPE dpT, ET_IN_TYPE ddpT, GenType time)
{
	_p0 = prev._pT;
	_dp0 = prev._dpT;
	_ddp0 = prev._ddpT;
	_pT = pT;
	_dpT = dpT;
	_ddpT = ddpT;
	_time = time;
	genPolynomial();
}

template <typename T, typename GenType>
Ce2FullyQualifiedSpline<T, GenType>::Ce2FullyQualifiedSpline(const Ce2PartialyQualifiedSpline<T, GenType>& partial)
{
	_p0 = partial.startPoint();
	_dp0 = partial.startPointVelocity();
	_ddp0 = partial.startPointAcceleration();
	_pT = partial.endPoint();
	_dpT = partial.endPointVelocity();
	_ddpT = partial.endPointAcceleration();
	_time = partial.duration();
	genPolynomial();
}

template <typename T, typename GenType>
Ce2FullyQualifiedSpline<T, GenType>::Ce2FullyQualifiedSpline(const Ce2MinimallyQualifiedSpline<T, GenType>& minimal)
{
	_p0 = minimal.startPoint();
	_dp0 = minimal.startPointVelocity();
	_ddp0 = minimal.startPointAcceleration();
	_pT = minimal.endPoint();
	_dpT = minimal.endPointVelocity();
	_ddpT = minimal.endPointAcceleration();
	_time = minimal.duration();
	genPolynomial();
}

template <typename T, typename GenType>
void Ce2FullyQualifiedSpline<T, GenType>::genPolynomial()
{
	this->_coefficients[0] = _p0;
	this->_coefficients[1] = _dp0;
	this->_coefficients[2] = _ddp0 * GenType(0.5);
	this->_coefficients[3] = _get_c3(_p0, _pT, _dp0, _dpT, _ddp0, _ddpT, _time);
	this->_coefficients[4] = _get_c4(_p0, _pT, _dp0, _dpT, _ddp0, _ddpT, _time);
	this->_coefficients[5] = _get_c5(_p0, _pT, _dp0, _dpT, _ddp0, _ddpT, _time);
}

template <typename T, typename GenType>
T Ce2FullyQualifiedSpline<T, GenType>::_get_c3(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, ET_IN_TYPE ddqT, GenType time) const
{
	GenType tp2 = time * time;
	return  (GenType( -1.5)*ddq0 + GenType( 0.5) * ddqT) / (time) +
		(GenType( -6.0)*dq0  + GenType(-4.0) * dqT)  / (tp2) +
		(GenType(-10.0)*q0   + GenType(10.0) * qT)   / (tp2 * time);
}

template <typename T, typename GenType>
T Ce2FullyQualifiedSpline<T, GenType>::_get_c4(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, ET_IN_TYPE ddqT, GenType time) const
{
	GenType tp2 = time*time;
	return (GenType(1.50) * ddq0 + GenType( -1.0)*ddqT) / (tp2) +
		(GenType( 8.0) * dq0  + GenType(  7.0)*dqT)  / (tp2 * time) +
		(GenType(15.0) * q0   + GenType(-15.0)*qT)   / (tp2 * tp2);
}

template <typename T, typename GenType>
T Ce2FullyQualifiedSpline<T, GenType>::_get_c5(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, ET_IN_TYPE ddqT, GenType time) const
{
	GenType tp2 = time*time;
	GenType tp4 = tp2 * tp2;
	return (GenType(-0.5)*ddq0 + GenType( 0.5)*ddqT) / (tp2 * time) +
		(GenType(-3.0)*dq0  + GenType(-3.0)*dqT)  / (tp2 * tp2) +
		(GenType(-6.0)*q0   + GenType( 6.0)*qT)   / (tp4 * time);
}

/**
* Minimally Qualified Spline
**/
template <typename T, typename GenType>
et::Ce2MinimallyQualifiedSpline<T, GenType>::Ce2MinimallyQualifiedSpline(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time)
{ 
	_p0 = p0;
	_dp0 = dp0;
	_pT = pT;
	_dpT = dpT;
	_time = time;
	genPolynomial();
}

template <typename T, typename GenType>
T et::Ce2MinimallyQualifiedSpline<T, GenType>::_get_c2(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time) const
{
	GenType tp2 = time * time;
	return (GenType(-1) * dpT + GenType(-2) * dp0) / time + 
		(GenType( 3) * pT  + GenType(-3) * p0)  / tp2;
}

template <typename T, typename GenType>
T et::Ce2MinimallyQualifiedSpline<T, GenType>::_get_c3(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time) const
{
	GenType tp2 = time * time;
	GenType tp3 = tp2 * time;
	return (GenType(1) * dp0 + GenType( 1) * dpT) / tp2 + 
		(GenType(2) * p0  + GenType(-2) * pT)  / tp3;
}

template <typename T, typename GenType>
void et::Ce2MinimallyQualifiedSpline<T, GenType>::genPolynomial()
{
	this->_coefficients[0] = _p0;
	this->_coefficients[1] = _dp0;
	this->_coefficients[2] = _get_c2(_p0, _dp0, _pT, _dpT, _time);
	this->_coefficients[3] = _get_c3(_p0, _dp0, _pT, _dpT, _time);
}

template <typename T, typename GenType>
const T et::Ce2MinimallyQualifiedSpline<T, GenType>::startPointAcceleration() const
{
	return this->derivative(2, 0);
}

template <typename T, typename GenType>
const T et::Ce2MinimallyQualifiedSpline<T, GenType>::midPointAcceleration() const
{
	return derivative(2, _time / GenType(2));
}

template <typename T, typename GenType>
const T et::Ce2MinimallyQualifiedSpline<T, GenType>::endPointAcceleration() const
{
	return derivative(2, _time);
}



/**
* Parialy Qualified Spline
**/
template <typename T, typename GenType>
et::Ce2PartialyQualifiedSpline<T, GenType>::Ce2PartialyQualifiedSpline(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE ddp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time)
{
	_p0 = p0;
	_dp0 = dp0;
	_ddp0 = ddp0;
	_pT = pT;
	_dpT = dpT;
	_time = time;
	genPolynomial();
}

template <typename T, typename GenType>
et::Ce2PartialyQualifiedSpline<T, GenType>::Ce2PartialyQualifiedSpline(const Ce2PartialyQualifiedSpline& prev, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time)
{
	_p0 = prev._pT;
	_dp0 = prev._dpT;
	_ddp0 = prev.endPointAcceleration();
	_pT = pT;
	_dpT = dpT;
	_time = time;
	genPolynomial();
}

template <typename T, typename GenType>
void Ce2PartialyQualifiedSpline<T, GenType>::genPolynomial()
{
	this->_coefficients[0] = _p0;
	this->_coefficients[1] = _dp0;
	this->_coefficients[2] = _ddp0 * GenType(0.5);
	this->_coefficients[3] = _get_c3(_p0, _pT, _dp0, _dpT, _ddp0, _time);
	this->_coefficients[4] = _get_c4(_p0, _pT, _dp0, _dpT, _ddp0, _time);
}

template <typename T, typename GenType>
T Ce2PartialyQualifiedSpline<T, GenType>::_get_c3(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, GenType time) const
{
	GenType tp2 = time * time;
	return (                               -ddq0) / (time) + 
		(           -dqT + GenType(-3) * dq0)  / (tp2) + 
		(GenType(4) * qT + GenType(-4) * q0)   / (tp2 * time);
} 

template <typename T, typename GenType>
T Ce2PartialyQualifiedSpline<T, GenType>::_get_c4(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, GenType time) const
{
	GenType tp2 = time * time;
	GenType tp4 = tp2 * tp2;
	return (GenType(6)*q0 - GenType(6)*qT + tp2*ddq0 + GenType(4)*time*dq0 + GenType(2)*time*dqT) / (GenType(2) * tp4);
}

template <typename T, typename GenType>
const T Ce2PartialyQualifiedSpline<T, GenType>::endPointAcceleration() const
{
	return derivative(2, _time);
}

/**
* Spline sequence
**/

template <typename T, typename GenType>
Ce2SplineSequence<T, GenType>::Ce2SplineSequence()
{
}

template <typename T, typename GenType>
Ce2SplineSequence<T, GenType>::Ce2SplineSequence(const Ce2SplinePointList& points)
{
	_points = points;
	buildSequence();
}

template <typename T, typename GenType>
Ce2SplineSequence<T, GenType>::Ce2SplineSequence(const Ce2SplineBasePointList& points)
{
	_points.resize(points.size());

	for (size_t i = 0, e = points.size(); i < e; ++i)
	{ 
		T vel0;
		T vel1;
		if (i + 1 < e)
		{
			vel0 = normalize(points[i+1].position - points[i].position);
			vel1 = (i > 0) ? normalize(points[i].position - points[i-1].position) : vel0;
		}
		else
		{
			vel0 = normalize(points[i].position - points[i-1].position);
			vel1 = vel0;
		}

		T acc = T(0);
		_points[i].position = points[i].position;
		_points[i].velocity = normalize(vel0 + vel1) * points[i].velocityModule;
		_points[i].acceleration = acc;
	}

	for (int k = 1, e = int(_points.size()) - 1; k != e; ++k)
	{ 
		int m = k - 1;
		int n = k + 1;
		GenType time_1 = GenType(2) * length(_points[k].position - _points[m].position) / (length(_points[k].velocity + _points[m].velocity));
		GenType time_2 = GenType(2) * length(_points[n].position - _points[k].position) / (length(_points[n].velocity + _points[k].velocity));
		Ce2MinimallyQualifiedSpline<T, GenType> mqs_1(_points[m].position, _points[m].velocity, _points[k].position, _points[k].velocity, time_1);
		Ce2MinimallyQualifiedSpline<T, GenType> mqs_2(_points[k].position, _points[k].velocity, _points[n].position, _points[n].velocity, time_2);
		_points[k].acceleration = GenType(0.5) * (mqs_1.endPointAcceleration() + mqs_2.startPointAcceleration());
	}

	buildSequence();
	/*
	std::vector<T> midpoint_accelerations;
	midpoint_accelerations.resize( _splines.size() );

	bool solved = false;
	GenType difference = 0;

	int iterations = 0;
	int max_iterations = 1000;
	while (!solved && (iterations++ < max_iterations))
	{
	int n = 0;
	difference = 0;

	for (Ce2FullyQualifiedSplineList::iterator i = _splines.begin(), e = _splines.end(); i != e; ++i, ++n)
	midpoint_accelerations[n] = i->midPointAcceleration();

	for (size_t i = 1, e = _points.size() - 1; i < e; ++i)
	{
	T average_acceleration = (midpoint_accelerations[i - 1] + midpoint_accelerations[i]) / GenType(2);
	difference += length(_points[i].acceleration - average_acceleration);
	_points[i].acceleration = average_acceleration;
	}

	buildSequence();
	solved = (difference <= GenType(1.0e-5));
	}
	*/
}

template <typename T, typename GenType>
void Ce2SplineSequence<T, GenType>::buildSequence()
{
	_splines.resize(_points.size() - 1);
	_duration = GenType(0);

	int k = 0;
	for (typename Ce2FullyQualifiedSplineList::iterator i = _splines.begin(), e = _splines.end(); i != e; ++i)
	{ 
		int m = k + 1;

		GenType time = GenType(2) * length(_points[k].position - _points[m].position) / (length(_points[k].velocity + _points[m].velocity));
		*i = Ce2FullyQualifiedSpline<T, GenType>(_points[k].position, _points[k].velocity, _points[k].acceleration, _points[m].position, _points[m].velocity, _points[m].acceleration, time);

		_duration += time;
		++k;
	}

	/*
	for (Ce2SplinePointList::iterator i = _points.begin(), e = _points.end() - 1; i != e; ++i)
	{
	Ce2SplinePointList::iterator j = i + 1;
	}
	*/
}

template <typename T, typename GenType>
typename Ce2SplineSequence<T, GenType>::Ce2SplinePoint Ce2SplineSequence<T, GenType>::sample(GenType time)
{
	typename Ce2SplineSequence<T, GenType>::Ce2SplinePoint result;
	result.position = T(0);
	result.velocity = T(0);
	result.acceleration = T(0);

	GenType local_time = time;
	for (typename Ce2FullyQualifiedSplineList::iterator i = _splines.begin(), e = _splines.end(); i != e; ++i)
	{
		if (local_time < i->duration())
		{
			result.position = i->derivative(0, local_time);
			result.velocity = i->derivative(1, local_time);
			result.acceleration = i->derivative(2, local_time);
			return result;
		}
		else
		{
			local_time -= i->duration();
		}
	}

	return result;
}

template <typename T, typename GenType>
typename Ce2SplineSequence<T, GenType>::Ce2SplinePoint splinePoint(T position, T velocity, T acceleration)
{
	typename Ce2SplineSequence<T, GenType>::Ce2SplinePoint result;
	result.position = position;
	result.velocity = velocity;
	result.acceleration = acceleration;
	return result;
}

template <typename T, typename GenType>
typename Ce2SplineSequence<T, GenType>::Ce2SplineBasePoint splineBasePoint(T position, GenType velocityModule)
{
	typename Ce2SplineSequence<T, GenType>::Ce2SplineBasePoint result;
	result.position = position;
	result.velocityModule = velocityModule;
	return result;
}

template <typename GenType>
typename Ce2SplineSequence<vector3<GenType>, GenType>::Ce2SplineBasePoint splineBasePoint(GenType x, GenType y, GenType z, GenType velocityModule)
{
	typename Ce2SplineSequence<vector3<GenType>, GenType>::Ce2SplineBasePoint result;
	result.position = vector3<GenType>(x, y, z);
	result.velocityModule = velocityModule;
	return result;
}

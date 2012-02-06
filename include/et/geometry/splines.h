#pragma once

#include <vector>

#define ET_IN_TYPE const T&

namespace et
{

	template <typename T, int size, typename GenType = T>
	class Ce2Polynome
	{
	public: 
		Ce2Polynome();
		Ce2Polynome(const T* coefficients);

		T calculate(GenType time) const;
		T derivative(int order, GenType time) const;
		T numericDerivative(int order, GenType time, GenType dt);
		T curvature(int derivativeOrder, GenType time) const;

	protected:
		T _coefficients[size];
	};

	template <typename T, typename GenType = T>
	class Ce2MinimallyQualifiedSpline : public Ce2Polynome<T, 5, GenType>
	{
	public:
		Ce2MinimallyQualifiedSpline(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time);

		inline const GenType duration() const {return _time;}

		inline const T startPoint() const {return _p0;}
		inline const T startPointVelocity() const {return _dp0;}

		inline const T endPoint() const {return _pT;}
		inline const T endPointVelocity() const {return _dpT;}

		const T startPointAcceleration() const;
		const T midPointAcceleration() const;
		const T endPointAcceleration() const;

	private:
		void genPolynomial();
		T _get_c2(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time) const;
		T _get_c3(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time) const;

	private:
		T _p0;
		T _dp0;
		T _pT;
		T _dpT;
		GenType _time;
	};

	template <typename T, typename GenType = T>
	class Ce2PartialyQualifiedSpline : public Ce2Polynome<T, 5, GenType>
	{
	public:
		Ce2PartialyQualifiedSpline(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE ddp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time);
		Ce2PartialyQualifiedSpline(const Ce2PartialyQualifiedSpline& prev, ET_IN_TYPE pT, ET_IN_TYPE dpT, GenType time);

		inline const GenType duration() const {return _time;}
		inline const T startPoint() const {return _p0;}
		inline const T startPointVelocity() const {return _dp0;}
		inline const T startPointAcceleration() const {return _ddp0;}
		inline const T endPoint() const {return _pT;}
		inline const T endPointVelocity() const {return _dpT;}

		const T endPointAcceleration() const;

	private: 
		void genPolynomial();
		T _get_c3(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, GenType time) const;
		T _get_c4(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, GenType time) const;

	private:
		T _p0;
		T _dp0;
		T _ddp0;
		T _pT;
		T _dpT;
		GenType _time;
	};

	template <typename T, typename GenType = T>
	class Ce2FullyQualifiedSpline : public Ce2Polynome<T, 6, GenType>
	{
	public:
		Ce2FullyQualifiedSpline(ET_IN_TYPE p0, ET_IN_TYPE dp0, ET_IN_TYPE ddp0, ET_IN_TYPE pT, ET_IN_TYPE dpT, ET_IN_TYPE ddpT, GenType time);

		Ce2FullyQualifiedSpline();
		Ce2FullyQualifiedSpline(const Ce2MinimallyQualifiedSpline<T, GenType>& minimal);
		Ce2FullyQualifiedSpline(const Ce2PartialyQualifiedSpline<T, GenType>& partial);
		Ce2FullyQualifiedSpline(const Ce2PartialyQualifiedSpline<T, GenType>& prev, ET_IN_TYPE pT, ET_IN_TYPE dpT, ET_IN_TYPE ddpT, GenType time);
		Ce2FullyQualifiedSpline(const Ce2FullyQualifiedSpline& prev, ET_IN_TYPE pT, ET_IN_TYPE dpT, ET_IN_TYPE ddpT, GenType time);

		inline const GenType duration() const {return _time;}
		inline const T startPoint() const {return _p0;}
		inline const T startPointVelocity() const {return _dp0;}
		inline const T startPointAcceleration() const {return _ddp0;}

		inline const T midPoint() const {return derivative(0, _time / GenType(2));}
		inline const T midPointVelocity() const {return derivative(1, _time / GenType(2));}
		inline const T midPointAcceleration() const {return derivative(2, _time / GenType(2));}

		inline const T endPoint() const {return _pT;}
		inline const T endPointVelocity() const {return _dpT;}
		inline const T endPointAcceleration() const {return _ddpT;}

	private:
		void genPolynomial();
		T _get_c3(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, ET_IN_TYPE ddqT, GenType time) const;
		T _get_c4(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, ET_IN_TYPE ddqT, GenType time) const;
		T _get_c5(ET_IN_TYPE q0, ET_IN_TYPE qT, ET_IN_TYPE dq0, ET_IN_TYPE dqT, ET_IN_TYPE ddq0, ET_IN_TYPE ddqT, GenType time) const;

	private:
		T _p0;
		T _dp0;
		T _ddp0;
		T _pT;
		T _dpT;
		T _ddpT;
		GenType _time;
	};

	template <typename T, typename GenType = T>
	class Ce2SplineSequence
	{
	public:
		struct Ce2SplinePoint
		{
			T position;
			T velocity;
			T acceleration;
		};
		struct Ce2SplineBasePoint
		{
			T position;
			GenType velocityModule;

			Ce2SplineBasePoint() : position(0), velocityModule(0) { }
			Ce2SplineBasePoint(T pos, GenType vel) : position(pos), velocityModule(vel) { }
		};
		typedef std::vector<Ce2SplinePoint> Ce2SplinePointList;
		typedef std::vector<Ce2SplineBasePoint> Ce2SplineBasePointList;
		typedef std::vector<Ce2FullyQualifiedSpline<T, GenType> > Ce2FullyQualifiedSplineList;

		Ce2SplineSequence();
		Ce2SplineSequence(const Ce2SplinePointList& points);
		Ce2SplineSequence(const Ce2SplineBasePointList& points);

		const GenType& duration() const {return _duration;}
		Ce2SplinePoint sample(GenType time);

	private:
		void buildSequence();

		bool _closedTrajectory;
		GenType _duration;
		Ce2SplinePointList _points;
		Ce2FullyQualifiedSplineList _splines;
	};

#include <et/geometry/splines.inl.h>

}

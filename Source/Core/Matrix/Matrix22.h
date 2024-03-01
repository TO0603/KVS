/****************************************************************************/
/**
 *  @file   Matrix22.h
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#pragma once
#include <iostream>
#include <sstream>
#include <iomanip>
#include <kvs/Assert>
#include <kvs/Math>
#include <kvs/BitArray>
#include <kvs/Indent>
#include <kvs/Vector2>
#include <kvs/Deprecated>


namespace kvs
{

/*==========================================================================*/
/**
 *  @brief  2x2 matrix class.
 */
/*==========================================================================*/
template<typename T>
class Matrix22
{
private:
    Vector2<T> m_data[2] = { {}, {} }; ///< Row vectors.

public:
    static const Matrix22 Zero() { Matrix22 m; m.setZero(); return m; }
    static const Matrix22 Ones() { Matrix22 m; m.setOnes(); return m; }
    static const Matrix22 Identity() { Matrix22 m; m.setIdentity(); return m; }
    static const Matrix22 Constant( const T x ) { Matrix22 m; m.setConstant(x); return m; }
    static const Matrix22 Diagonal( const T x ) { Matrix22 m; m.setDiagonal(x); return m; }
    static const Matrix22 Diagonal( const Vector2<T>& v ) { Matrix22 m; m.setDiagonal(v); return m; }
    static const Matrix22 Random() { Matrix22 m; m.setRandom(); return m; }
    static const Matrix22 Random( const kvs::UInt32 seed ) { Matrix22 m; m.setRandom( seed ); return m; }
    static const Matrix22 Random( const T min, const T max ) { Matrix22 m; m.setRandom( min, max ); return m; }
    static const Matrix22 Random( const T min, const T max, const kvs::UInt32 seed ) { Matrix22 m; m.setRandom( min, max, seed ); return m; }
    static const Matrix22 Rotation( const double deg );

public:
    Matrix22() = default;
    Matrix22(
        const T a00, const T a01,
        const T a10, const T a11 );
    Matrix22(
        const Vector2<T>& v0,
        const Vector2<T>& v1 );
    explicit Matrix22( const T elements[4] );

    template <
        typename T00, typename T01,
        typename T10, typename T11>
    Matrix22(
        const T00 a00, const T01 a01,
        const T10 a10, const T11 a11 );

    void set(
        const T a00, const T a01,
        const T a10, const T a11 );
    void set(
        const Vector2<T>& v0,
        const Vector2<T>& v1 );
    void set( const T elements[4] );
    void setZero();
    void setOnes();
    void setIdentity();
    void setConstant( const T x );
    void setDiagonal( const T x );
    void setDiagonal( const kvs::Vector2<T>& v );
    void setRandom();
    void setRandom( const kvs::UInt32 seed );
    void setRandom( const T min, const T max );
    void setRandom( const T min, const T max, const kvs::UInt32 seed );

    void swap( Matrix22& other );
    void transpose();
    void invert( T* determinant = 0 );

    T trace() const;
    T determinant() const;
    const Matrix22 transposed() const;
    const Matrix22 inverted( T* determinant = 0 ) const;
    size_t rank() const;
    bool isSymmetric() const;
    bool isDiagonal() const;

    std::string format(
        const bool newline = false,
        const kvs::Indent& indent = kvs::Indent(0) ) const
    {
        return this->format( ", ", "[", "]", newline, indent );
    }

    std::string format(
        const std::string delim,
        const bool newline = false,
        const kvs::Indent& indent = kvs::Indent(0) ) const
    {
        return this->format( delim, "", "", newline, indent );
    }

    std::string format(
        const std::string bracket_l,
        const std::string bracket_r,
        const bool newline = false,
        const kvs::Indent& indent = kvs::Indent(0) ) const
    {
        return this->format( " ", bracket_l, bracket_r, newline, indent );
    }

    std::string format(
        const std::string delim,
        const std::string bracket_l,
        const std::string bracket_r,
        const bool newline = false,
        const kvs::Indent& indent = kvs::Indent(0) ) const;

public:
    const Vector2<T>& operator []( const size_t index ) const;
    Vector2<T>& operator []( const size_t index );
    Matrix22& operator +=( const Matrix22& rhs );
    Matrix22& operator -=( const Matrix22& rhs );
    Matrix22& operator *=( const Matrix22& rhs );
    Matrix22& operator *=( const T rhs );
    Matrix22& operator /=( const T rhs );
    const Matrix22 operator -() const;

    friend bool operator ==( const Matrix22& lhs, const Matrix22& rhs )
    {
        return lhs[0] == rhs[0] &&
               lhs[1] == rhs[1];
    }

    friend bool operator !=( const Matrix22& lhs, const Matrix22& rhs )
    {
        return !( lhs == rhs );
    }

    friend const Matrix22 operator +( const Matrix22& lhs, const Matrix22& rhs )
    {
        return Matrix22( lhs ) += rhs;
    }

    friend const Matrix22 operator -( const Matrix22& lhs, const Matrix22& rhs )
    {
        return Matrix22( lhs ) -= rhs;
    }

    friend const Matrix22 operator *( const Matrix22& lhs, const Matrix22& rhs )
    {
        return Matrix22( lhs ) *= rhs;
    }

    friend const Vector2<T> operator *( const Matrix22& lhs, const Vector2<T>& rhs )
    {
        return Vector2<T>(
            lhs[0].dot( rhs ),
            lhs[1].dot( rhs ) );
    }

    friend const Vector2<T> operator *( const Vector2<T>& lhs, const Matrix22& rhs )
    {
        return Vector2<T>(
            lhs[0] * rhs[0][0] + lhs[1] * rhs[1][0],
            lhs[0] * rhs[0][1] + lhs[1] * rhs[1][1] );
    }

    friend const Matrix22 operator *( const Matrix22& lhs, const T rhs )
    {
        return Matrix22( lhs ) *= rhs;
    }

    friend const Matrix22 operator *( const T lhs, const Matrix22& rhs )
    {
        return rhs * lhs;
    }

    friend const Matrix22 operator /( const Matrix22& lhs, const T rhs )
    {
        return Matrix22( lhs ) /= rhs;
    }

    friend std::ostream& operator <<( std::ostream& os, const Matrix22& rhs )
    {
        return os << rhs.format( " ", "", "", true );
    }

public:
    KVS_DEPRECATED( static const Matrix22 All( const T x ) ) { return Constant( x ); }
    KVS_DEPRECATED( explicit Matrix22( const T a ) ) { *this = Constant( a ); }
    KVS_DEPRECATED( void set( const T a ) ) { *this = Constant( a ); }
    KVS_DEPRECATED( void zero() ) { this->setZero(); }
    KVS_DEPRECATED( void identity() ) { this->setIdentity(); }
    KVS_DEPRECATED( void print() const ) { std::cout << *this << std::endl; }
};


/*==========================================================================*/
/**
 *  Type definition.
 */
/*==========================================================================*/
using Mat2 = Matrix22<float>;
using Mat2f = Matrix22<float>;
using Mat2d = Matrix22<double>;

using Matrix22f = Matrix22<float>;
using Matrix22d = Matrix22<double>;


template<typename T>
const Matrix22<T> Matrix22<T>::Rotation( const double deg )
{
    const T rad = static_cast<T>( kvs::Math::Deg2Rad( deg ) );
    const T sinA = static_cast<T>( std::sin( rad ) );
    const T cosA = static_cast<T>( std::cos( rad ) );
    return Matrix22(
        cosA, -sinA,
        sinA,  cosA );
}

/*==========================================================================*/
/**
 *  @brief  Constructs a new Matrix22.
 *  @param  a00 [in] Element.
 *  @param  a01 [in] Element.
 *  @param  a10 [in] Element.
 *  @param  a11 [in] Element.
 */
/*==========================================================================*/
template<typename T>
inline Matrix22<T>::Matrix22(
    const T a00, const T a01,
    const T a10, const T a11 ):
    m_data{ { a00, a01 }, { a10, a11 } }
{
}

/*==========================================================================*/
/**
 *  @brief  Constructs a new Matrix22.
 *  @param  v0 [in] 1st row vector.
 *  @param  v1 [in] 2nd row vector.
 */
/*==========================================================================*/
template<typename T>
inline Matrix22<T>::Matrix22(
    const Vector2<T>& v0,
    const Vector2<T>& v1 ):
    m_data{ v0, v1 }
{
}

/*==========================================================================*/
/**
 *  @brief  Constructs a new Matrix22.
 *  @param  elements [in] Array of elements.
 */
/*==========================================================================*/
template<typename T>
inline Matrix22<T>::Matrix22( const T elements[4] ):
    m_data{
        { elements[0], elements[1] },
        { elements[2], elements[3] } }
{
}

template <typename T>
template <
    typename T00, typename T01,
    typename T10, typename T11>
inline Matrix22<T>::Matrix22(
    const T00 a00, const T01 a01,
    const T10 a10, const T11 a11 ):
    m_data{
        { static_cast<T>(a00), static_cast<T>(a01) },
        { static_cast<T>(a10), static_cast<T>(a11) } }
{
}

/*==========================================================================*/
/**
 *  @brief  Sets the elements.
 *  @param  a00 [in] Element.
 *  @param  a01 [in] Element.
 *  @param  a10 [in] Element.
 *  @param  a11 [in] Element.
 */
/*==========================================================================*/
template<typename T>
inline void Matrix22<T>::set(
    const T a00, const T a01,
    const T a10, const T a11 )
{
    m_data[0].set( a00, a01 );
    m_data[1].set( a10, a11 );
}

/*==========================================================================*/
/**
 *  @brief  Sets the elements.
 *  @param  v0 [in] 1st row vector.
 *  @param  v1 [in] 2nd row vector.
 */
/*==========================================================================*/
template<typename T>
inline void Matrix22<T>::set(
    const Vector2<T>& v0,
    const Vector2<T>& v1 )
{
    m_data[0] = v0;
    m_data[1] = v1;
}

/*==========================================================================*/
/**
 *  @brief  Sets the elements.
 *  @param  elements [in] Array of elements.
 */
/*==========================================================================*/
template<typename T>
inline void Matrix22<T>::set( const T elements[4] )
{
    m_data[0].set( elements     );
    m_data[1].set( elements + 2 );
}

template<typename T>
inline void Matrix22<T>::setZero()
{
    m_data[0].setZero();
    m_data[1].setZero();
}

template<typename T>
inline void Matrix22<T>::setOnes()
{
    m_data[0].setOnes();
    m_data[1].setOnes();
}

template<typename T>
inline void Matrix22<T>::setIdentity()
{
    m_data[0].setUnitX();
    m_data[1].setUnitY();
}

template<typename T>
inline void Matrix22<T>::setConstant( const T x )
{
    m_data[0].setConstant(x);
    m_data[1].setConstant(x);
}

template<typename T>
inline void Matrix22<T>::setDiagonal( const T x )
{
    m_data[0].setZero(); m_data[0][0] = x;
    m_data[1].setZero(); m_data[1][1] = x;
}

template<typename T>
inline void Matrix22<T>::setDiagonal( const kvs::Vector2<T>& v )
{
    m_data[0].setZero(); m_data[0][0] = v[0];
    m_data[1].setZero(); m_data[1][1] = v[1];
}

template<typename T>
inline void Matrix22<T>::setRandom()
{
    m_data[0].setRandom();
    m_data[1].setRandom();
}

template<typename T>
inline void Matrix22<T>::setRandom( const kvs::UInt32 seed )
{
    m_data[0].setRandom( seed );
    m_data[1].setRandom();
}

template<typename T>
inline void Matrix22<T>::setRandom( const T min, const T max )
{
    m_data[0].setRandom( min, max );
    m_data[1].setRandom( min, max );
}

template<typename T>
inline void Matrix22<T>::setRandom( const T min, const T max, const kvs::UInt32 seed )
{
    m_data[0].setRandom( min, max, seed );
    m_data[1].setRandom( min, max );
}

/*==========================================================================*/
/**
 *  @brief  Swaps this and other.
 *  @param  other [in,out] Matrix22.
 */
/*==========================================================================*/
template<typename T>
inline void Matrix22<T>::swap( Matrix22& other )
{
    m_data[0].swap( other[0] );
    m_data[1].swap( other[1] );
}

/*==========================================================================*/
/**
 *  @brief  Transposes this matrix.
 *  @return Transposed matrix.
 */
/*==========================================================================*/
template<typename T>
inline void Matrix22<T>::transpose()
{
    std::swap( m_data[0][1], m_data[1][0] );
}

/*==========================================================================*/
/**
 *  @brief  Inverts this matrix.
 *  @param  determinant [out] calculated determinant
 *  @return Inverse matrix.
 */
/*==========================================================================*/
template<typename T>
inline void Matrix22<T>::invert( T* determinant )
{
    const T det22 = m_data[0][0] * m_data[1][1] - m_data[0][1] * m_data[1][0];

    if ( determinant ) *determinant = det22;

    // Inverse.
    this->set(
        +m_data[1][1], -m_data[0][1],
        -m_data[1][0], +m_data[0][0] );

    const T det_inverse = static_cast<T>( 1.0 / det22 );
    *this *= det_inverse;
}

/*==========================================================================*/
/**
 *  @brief  Prints the elements as a formatted string.
 *  @param  delim [in] delimiter
 *  @param  bracket_l [in] left bracket
 *  @param  bracket_r [in] right bracket
 *  @param  newline [in] flag for newline for each row
 *  @param  indent [in] indent for each row
 */
/*==========================================================================*/
template<typename T>
inline std::string Matrix22<T>::format(
    const std::string delim,
    const std::string bracket_l,
    const std::string bracket_r,
    const bool newline,
    const kvs::Indent& indent ) const
{
    std::ostringstream os;
    os << indent << bracket_l;
    {
        const std::string offset( bracket_l.size(), ' ' );
        os << m_data[0].format( delim, bracket_l, bracket_r );
        os << delim; if ( newline ) { os << std::endl << indent << offset; }
        os << m_data[1].format( delim, bracket_l, bracket_r );
    }
    os << bracket_r;
    return os.str();
}

/*==========================================================================*/
/**
 *  @brief  Calculates the trace of this matrix.
 *  @return Trace of this matrix.
 */
/*==========================================================================*/
template<typename T>
inline T Matrix22<T>::trace() const
{
    return m_data[0][0] + m_data[1][1];
}

/*==========================================================================*/
/**
 *  @brief  Calculates the determinant of this matrix.
 *  @return Determinant of this matrix.
 */
/*==========================================================================*/
template<typename T>
inline T Matrix22<T>::determinant() const
{
    const T det22 = m_data[0][0] * m_data[1][1] - m_data[0][1] * m_data[1][0];
    return det22;
}

/*==========================================================================*/
/**
 *  @brief  Copies this and transposes it.
 *  @return Transposed matrix.
 */
/*==========================================================================*/
template<typename T>
inline const Matrix22<T> Matrix22<T>::transposed() const
{
    Matrix22 result( *this );
    result.transpose();
    return result;
}

/*==========================================================================*/
/**
 *  @brief  Copies this and inverts it.
 *  @param  determinant [out] calculated determinant
 *  @return Inverse matrix.
 */
/*==========================================================================*/
template<typename T>
inline const Matrix22<T> Matrix22<T>::inverted( T* determinant ) const
{
    Matrix22 result( *this );
    result.invert( determinant );
    return result;
}

/*===========================================================================*/
/**
 *  @brief  Returns the rank of this matrix.
 *  @return rank of this matrix
 */
/*===========================================================================*/
template<typename T>
inline size_t Matrix22<T>::rank() const
{
    const size_t nrows = 2;
    const size_t ncols = 2;

    kvs::Matrix22<T> tmp = *this;
    kvs::BitArray flags( nrows, false );

    size_t r = 0;
    for ( size_t i = 0; i < ncols; ++i )
    {
        size_t j = 0;
        for ( j = 0; j < nrows; ++j )
        {
            if ( !flags[j] && !kvs::Math::IsZero( tmp[j][i] ) ) { break; }
        }

        if ( j != nrows )
        {
            ++r;
            flags.set(j);
            for ( size_t s = i + 1; s < ncols; ++s )
            {
                tmp[j][s] /= tmp[j][i];
            }
            for ( size_t s = 0; s < nrows; ++s )
            {
                if ( s != j && !kvs::Math::IsZero( tmp[s][i] ) )
                {
                    for ( size_t t = i + 1; t < ncols; ++t )
                    {
                        tmp[s][t] -= tmp[j][t] * tmp[s][i];
                    }
                }
            }
        }
    }
    return r;
}

template<typename T>
inline bool Matrix22<T>::isSymmetric() const
{
    if ( !kvs::Math::Equal( m_data[0][1], m_data[1][0] ) ) { return false; }
    return true;
}

template<typename T>
inline bool Matrix22<T>::isDiagonal() const
{
    if ( !kvs::Math::IsZero( m_data[0][1] ) ||
         !kvs::Math::IsZero( m_data[1][0] ) ) { return false; }
    return true;
}

template<typename T>
inline const Vector2<T>& Matrix22<T>::operator []( const size_t index ) const
{
    KVS_ASSERT( index < 2 );
    return m_data[ index ];
}

template<typename T>
inline Vector2<T>& Matrix22<T>::operator []( const size_t index )
{
    KVS_ASSERT( index < 2 );
    return m_data[ index ];
}

template<typename T>
inline Matrix22<T>& Matrix22<T>::operator +=( const Matrix22& rhs )
{
    m_data[0] += rhs[0];
    m_data[1] += rhs[1];
    return *this;
}

template<typename T>
inline Matrix22<T>& Matrix22<T>::operator -=( const Matrix22& rhs )
{
    m_data[0] -= rhs[0];
    m_data[1] -= rhs[1];
    return *this;
}

template<typename T>
inline Matrix22<T>& Matrix22<T>::operator *=( const Matrix22& rhs )
{
    this->set(
        m_data[0][0] * rhs[0][0] + m_data[0][1] * rhs[1][0],
        m_data[0][0] * rhs[0][1] + m_data[0][1] * rhs[1][1],
        m_data[1][0] * rhs[0][0] + m_data[1][1] * rhs[1][0],
        m_data[1][0] * rhs[0][1] + m_data[1][1] * rhs[1][1] );
    return *this;
}

template<typename T>
inline Matrix22<T>& Matrix22<T>::operator *=( const T rhs )
{
    m_data[0] *= rhs;
    m_data[1] *= rhs;
    return *this;
}

template<typename T>
inline Matrix22<T>& Matrix22<T>::operator /=( const T rhs )
{
    m_data[0] /= rhs;
    m_data[1] /= rhs;
    return *this;
}

template<typename T>
inline const Matrix22<T> Matrix22<T>::operator -() const
{
    return Matrix22( *this ) *= T(-1);
}

} // end of namespace kvs

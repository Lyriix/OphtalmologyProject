#ifndef __itkLineShapeImageFilter_h
#define __itkLineShapeImageFilter_h

// C++ includes

#include <iostream>

#include "itkNumericTraits.h"
#include "itkCrossHelper.h"

// Image includes

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

// Filter includes

#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkHessian3DToVesselnessMeasureImageFilter.h"

// Eigen values processing

#include "itkSymmetricEigenAnalysis.h"

// Functor

#include "itkUnaryFunctorImageFilter.h"

namespace itk 
{
/** \class LineShapeImageFilter
 * \brief Extract fiber-like shapes based on a convexity criterion
 * 
 * The filter takes as input a grayscale image.
 * It calculates a Gaussian blurred Hessian image and uses its eigenvalues 
 * to determine wether a pixel is likely to be a fiber or not.
 * Let \f$ |\lambda_3| > |\lambda_2| > |\lambda_1|\f$ : 
 *
 * \li Dark fiber-like shapes would have \f$\lambda_3>0\f$ and \f$\labmda_2>0\f$ \f$a\f$
 * \li Bright fiber-like shapes would have \f$\lambda_3<0\f$ and \f$\labmda_2<0\f$
 *
 * The boolean m_ExtractBrightLike determines if we extract bright or dark lines.
 * The float m_Sigma determines the size of the Gaussian kernel for blurring
 *
 * This filter can calculate multiple images, which are all by default desactivated.
 * It can get an image of each eigenvalue, sorted as explained before.
 * It can get a labeled image with 4 labels in the case of fibers extraction in 3D.
 * A label is define binary as follow : label = ( \f$(\lambda_2>0)\f$ \f$(\lambda_3>0)\f$ )\f$_2\f$
 *
 * \sa HessianRecursiveGaussianImageFilter
 * \sa Hessian3DToVesselnessMeasureImageFilter
 *
 * \ingroup ITKImageFeature
 *
 * \ingroup IntensityImageFilters
 *
 */
template< class TInputImage, class TOutputImage = TInputImage, class TLabelImage = TOutputImage, class TVectorImage = Image< Vector< typename NumericTraits< typename TInputImage::PixelType >::RealType, TInputImage::ImageDimension >, TInputImage::ImageDimension > >

class ITK_EXPORT LineShapeImageFilter :
	public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
	/** Standard class typedefs. */
	typedef LineShapeImageFilter 								Self;
	typedef ImageToImageFilter< TInputImage, TOutputImage >		Superclass;
	typedef SmartPointer< Self >								Pointer;
	typedef SmartPointer< const Self >							ConstPointer;

	/** Method for creation through the object factory. */
	itkNewMacro( Self );

	/** Run-time type information (and related methods). */
	itkTypeMacro( LineShapeImageFilter, ImageToImageFilter );

	/** Convenient typedef. */
	typedef typename Superclass::InputImageType					InputImageType;
	typedef typename Superclass::InputImageConstPointer			InputImageConstPointer;
	typedef typename Superclass::InputImagePixelType			InputImagePixelType;
	typedef typename Superclass::InputImagePointer				InputImagePointer;
	typedef typename Superclass::InputImageRegionType			InputImageRegionType;
	typedef typename Superclass::OutputImageType				OutputImageType;
	typedef typename Superclass::OutputImagePixelType			OutputImagePixelType;
	typedef typename Superclass::OutputImagePointer				OutputImagePointer;
	typedef typename Superclass::OutputImageRegionType			OutputImageRegionType;
	typedef TLabelImage											LabelImageType;
	typedef typename LabelImageType::PixelType 					LabelType;
	typedef typename LabelImageType::Pointer					LabelImagePointer;
	typedef TVectorImage										VectorImageType;
	typedef typename VectorImageType::PixelType					VectorType;

	typedef float		RealType;

	static const unsigned int ImageDimension = InputImageType::ImageDimension;

	typedef Image< RealType, ImageDimension >					RealImageType;

	/** Binary output. */
	OutputImageType* GetBinaryOutput();
	/** Label output, disactivated by default. */
	LabelImageType* GetLabelOutput();
	/** Eigenvectors images output. Disactivated by default.
	 * It calculates the cross product between the two highest eigenvalues. */
	VectorImageType* GetLineDirectionOutput();
	/** Eigenvalues images output. Disactivated by default. */
	RealImageType* GetEigenValuesOutput( unsigned int i );
	/** Vesselness image output. */
	RealImageType* GetVesselnessOutput();

	/** Get macro to get the value of a Brigh / Dark line. */
	itkGetConstMacro( BrightLine, LabelType );
	itkGetConstMacro( DarkLine, LabelType );

	/** Get macro to get the number of disconnected objects (available
	 * if label image is activated.
	 */
	itkGetConstMacro( LabelCount, LabelType );

	/** Set/Get macros to determine if we extract bright of dark lines (default false). */
	itkGetConstMacro( ExtractBrightLine, bool );
	itkSetMacro( ExtractBrightLine, bool );

	/** Set/Get macros to enable/disable eigenvalues image outputs (default false). */
	itkGetConstMacro( EigenValuesExtraction, bool );
	itkSetMacro( EigenValuesExtraction, bool );

	/** Set/Get macros to enable/disable label image output (default false). */
	itkGetConstMacro( LabelImage, bool );
	itkSetMacro( LabelImage, bool );

	/** GetRegion macro. */
	itkGetConstMacro( Region, InputImageRegionType );

	/** Set/Get macros to determine Sigma in mm (default 1.0f). */
	itkGetConstMacro( Sigma, RealType );
	itkSetMacro( Sigma, RealType );

	/** Set/Get macros to define outside value for the binary output. */
	itkGetConstMacro( OutsideValue, OutputImagePixelType );
	itkSetMacro( OutsideValue, OutputImagePixelType );

	/** Set/Get macros to define inside value for the binary output. */
	itkGetConstMacro( InsideValue, OutputImagePixelType );
	itkSetMacro( InsideValue, OutputImagePixelType );

	/** Set/Get macros to determine the number of dimensions we process.
	 * By default, it is 2 (2 eigenvalues) for fiber shapes.
	 * 1 would get plane shapes.
	 */
	itkGetConstMacro( DimensionsProcessed, unsigned int );
	itkSetMacro( DimensionsProcessed, unsigned int );

	/** Activates / Desactivates eigenvalues outputs (default false). */
	void EigenValuesExtractionOn();
	void EigenValuesExtractionOff();
	/** Activates / Desactivates label image output (default false). */
	void LabelImageOn();
	void LabelImageOff();

protected:
	/** Filter typedefs. */
	typedef HessianRecursiveGaussianImageFilter< InputImageType >			HessianGaussianFilterType;
	typedef BinaryThresholdImageFilter< LabelImageType, OutputImageType >	BinaryThresholdFilterType;
	typedef BinaryThresholdImageFilter< RealImageType, RealImageType >		RealBinaryThresholdFilterType;
	typedef Hessian3DToVesselnessMeasureImageFilter< RealType >			VesselnessFilterType;

	/** Inside pixel typedefs. */
	typedef typename HessianGaussianFilterType::OutputImageType				TensorImageType;
	typedef typename HessianGaussianFilterType::OutputPixelType				TensorType;
	typedef typename TensorType::EigenValuesArrayType						EigenValuesArrayType;
	typedef typename TensorType::EigenVectorsMatrixType						EigenVectorsMatrixType;
	typedef Image< EigenValuesArrayType, ImageDimension >					EigenValuesArrayImageType;

	/** Matrix analysis tool typedef. */
	typedef SymmetricEigenAnalysis< TensorType, EigenValuesArrayType >			EigenAnalysisType;

	/** Functor classes. */
	class AbsoluteSortedEigenValuesArrayExtractor
	{
		public:
			AbsoluteSortedEigenValuesArrayExtractor() {};
			~AbsoluteSortedEigenValuesArrayExtractor() {};
			bool operator != ( const AbsoluteSortedEigenValuesArrayExtractor & ) const { return false; }
			bool operator == ( const AbsoluteSortedEigenValuesArrayExtractor & other ) const { return !( *this != other ); }
			inline EigenValuesArrayType operator()( const TensorType & A ) const;
	};

	class SignedBasedEigenValuesClassifier
	{
		public:
			SignedBasedEigenValuesClassifier() { m_DimensionsProcessed = ImageDimension - 1; }
			~SignedBasedEigenValuesClassifier() {};
			bool operator != ( const SignedBasedEigenValuesClassifier & ) const { return false; }
			bool operator == ( const SignedBasedEigenValuesClassifier & other ) const { return *this != other; }
			void SetDimensionsProcessed( unsigned int & dimensionsProcessed ) { m_DimensionsProcessed = dimensionsProcessed; }
			unsigned int GetDimensionsProcessed() const { return m_DimensionsProcessed; }
			inline LabelType operator() ( const EigenValuesArrayType & A ) const;
		protected:
			unsigned int m_DimensionsProcessed;
	};

	class EigenValuesExtractor
	{
		public:
			EigenValuesExtractor() { m_EigenIndex = 0; }
			~EigenValuesExtractor() {};
			bool operator != ( const EigenValuesExtractor & ) const { return false; }
			bool operator == ( const EigenValuesExtractor & other ) const { return *this != other; }
			void SetEigenIndex( unsigned int & eigenIndex ) { m_EigenIndex = eigenIndex; }
			unsigned int GetEigenIndex() const { return m_EigenIndex; }
			inline RealType operator() ( const EigenValuesArrayType & A ) const;
		protected:
			unsigned int m_EigenIndex;
	};
	
	class LineDirectionVectorExtractor
	{
		public:
			LineDirectionVectorExtractor() {}
			~LineDirectionVectorExtractor() {};
			bool operator != ( const LineDirectionVectorExtractor & ) const { return false; }
			bool operator == ( const LineDirectionVectorExtractor & other ) const { return *this != other; }
			inline VectorType operator() ( const TensorType & A ) const;
	};

	class NotOperator
	{
		public:
			NotOperator() {};
			~NotOperator() {};
			bool operator != ( const NotOperator & ) const { return false; }
			bool operator == ( const NotOperator & other ) const { return *this != other; }
			inline PixelType operator() ( const PixelType & A ) const
			{
				return ~A;
			}
	};

	/** Functor typedef. */
	typedef UnaryFunctorImageFilter< TensorImageType, EigenValuesArrayImageType, AbsoluteSortedEigenValuesArrayExtractor >	EigenValuesArrayFilterType;
	typedef UnaryFunctorImageFilter< EigenValuesArrayImageType, LabelImageType, SignedBasedEigenValuesClassifier >			LabelFilterType;
	typedef UnaryFunctorImageFilter< EigenValuesArrayImageType, RealImageType, EigenValuesExtractor >						EigenValuesFilterType;
	typedef UnaryFunctorImageFilter< TensorImageType, VectorImageType, LineDirectionVectorExtractor >						LineDirectionFilterType;
	typedef UnaryFunctorImageFilter< ImageType, ImageType, NotOperator >													NotFilterType;

	LineShapeImageFilter();

	/** GenerateData function. */
	void GenerateData();

	/** ComputeData function. */
	void ComputeData();

	DataObject::Pointer MakeOutput( unsigned int idx );

	virtual ~LineShapeImageFilter() {}
//	void PrintSelf( std::ostream & s, Indent indent ) const;

	/* Sets region. */
	void SetRegion( const InputImageRegionType & region );

private:
	LineShapeImageFilter( const Self & );	// purposely not implemented
	void operator=( const Self & );			// purposely not implemented

	InputImageRegionType m_Region;
	bool m_RegionSetByUser;

	LabelType m_BrightLine;
	LabelType m_DarkLine;
	LabelType m_LabelCount;

	bool m_ExtractBrightLine;

	bool m_EigenValuesExtraction;
	bool m_LabelImage;

	OutputImagePixelType m_OutsideValue;
	OutputImagePixelType m_InsideValue;

	unsigned int m_DimensionsProcessed;

	RealType m_Sigma;

	/** Compare class, \f$x < y\f$ iff \f$|x| < |y|\f$. */
	class Compare
	{
		public:
			Compare() {};
			~Compare() {};
			bool operator()( const RealType x, const RealType y ) const
			{ return std::abs( x ) < std::abs( y ); }
	};
};
}// namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLineShapeImageFilter.hxx"
#endif

#endif

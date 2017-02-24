#ifndef __itkLineShapeImageFilter_hxx
#define __itkLineShapeImageFilter_hxx

#include "itkLineShapeImageFilter.h"

namespace itk {

// Constructor
template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::LineShapeImageFilter()
{
	this->SetNumberOfRequiredOutputs( 6 );

	this->SetNthOutput( 0, this->MakeOutput( 0 ) );
	this->SetNthOutput( 1, this->MakeOutput( 1 ) );
	this->SetNthOutput( 2, this->MakeOutput( 2 ) );
	this->SetNthOutput( 3, this->MakeOutput( 3 ) );
	this->SetNthOutput( 4, this->MakeOutput( 4 ) );
	this->SetNthOutput( 5, this->MakeOutput( 5 ) );
	this->SetNthOutput( 6, this->MakeOutput( 6 ) );

	m_DarkLine = 0;
	m_LabelCount = 1 << ( ImageDimension - 1 );
	m_BrightLine = m_LabelCount - 1;

	m_Sigma = 2.0f;

	m_ExtractBrightLine = false;

	m_EigenValuesExtraction = false;
	m_LabelImage = false;

	m_DimensionsProcessed = ImageDimension - 1;

	m_OutsideValue = NumericTraits< OutputImagePixelType >::Zero;
	m_InsideValue = NumericTraits< OutputImagePixelType >::max();

	m_RegionSetByUser = false;
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
void LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::GenerateData()
{
	this->AllocateOutputs();
	if( !m_RegionSetByUser )
		m_Region = this->GetInput()->GetLargestPossibleRegion();

	this->ComputeData();
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
void LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::ComputeData()
{
	// Vesselness filter only extract bright fibers, so we use
	// if we are looking for dark fibers

	typename NotFilterType::Pointer notFilter;
	if( !m_ExtractBrightLine )
	{
		notFilter = NotFilterType::New();
		notFilter->SetInput( this->GetInput() );
		notFilter->Update();
	}

	// Hessian Gaussian filtering
	typename HessianGaussianFilterType::Pointer hessianGaussianFilter = HessianGaussianFilterType::New();
	if( !m_ExtractBrightLine )
	{
		hessianGaussianFilter->SetInput( notFilter->GetOutput() );
	}
	else
	{
		hessianGaussianFilter->SetInput( this->GetInput() );
	}
	hessianGaussianFilter->SetSigma( m_Sigma );
	hessianGaussianFilter->Update();

	// Eigen values extraction
	typename EigenValuesArrayFilterType::Pointer eigenValuesArrayFilter = EigenValuesArrayFilterType::New();
	eigenValuesArrayFilter->SetInput( hessianGaussianFilter->GetOutput() );
	eigenValuesArrayFilter->Update();

	// Label filtering
	typename LabelFilterType::Pointer labelFilter = LabelFilterType::New();
	labelFilter->SetInput( eigenValuesArrayFilter->GetOutput() );
	labelFilter->GetFunctor().SetDimensionsProcessed( m_DimensionsProcessed );
	labelFilter->Update();

	// Fiber-like shape extraction
	typename BinaryThresholdFilterType::Pointer thresholdFilter = BinaryThresholdFilterType::New();
	thresholdFilter->SetInput( labelFilter->GetOutput() );
	thresholdFilter->SetOutsideValue( m_OutsideValue );
	thresholdFilter->SetInsideValue( m_InsideValue );
	thresholdFilter->SetUpperThreshold( m_DarkLine );
	thresholdFilter->SetLowerThreshold( m_DarkLine );
	thresholdFilter->Update();

	// Eigenvalues output
	if( m_EigenValuesExtraction )
	{
		for( unsigned int i = 0; i < ImageDimension; ++i )
		{
			typename EigenValuesFilterType::Pointer eigenValuesFilter = EigenValuesFilterType::New();
			eigenValuesFilter->SetInput( eigenValuesArrayFilter->GetOutput() );
			eigenValuesFilter->GetFunctor().SetEigenIndex( i );
			eigenValuesFilter->Update();
			this->GraftNthOutput( 2 + i, eigenValuesFilter->GetOutput() );
		}
	}

	if( m_EigenValuesExtraction )
	{
		typename LineDirectionFilterType::Pointer lineDirectionFilter = LineDirectionFilterType::New();
		lineDirectionFilter->SetInput( hessianGaussianFilter->GetOutput() );
		lineDirectionFilter->Update();

		this->GraftNthOutput( 5, lineDirectionFilter->GetOutput() );
	}

	// Label image output
	if( m_LabelImage )
	{
		this->GraftNthOutput( 1, labelFilter->GetOutput() );
	}

	// Vesselness filter
	typename VesselnessFilterType::Pointer vesselnessFilter = VesselnessFilterType::New();
	vesselnessFilter->SetInput( hessianGaussianFilter->GetOutput() );
	vesselnessFilter->SetAlpha1( 0.5 );
	vesselnessFilter->SetAlpha2( 2.0 );
	vesselnessFilter->Update();

	this->GraftNthOutput( 6, vesselnessFilter->GetOutput() );

	this->GraftNthOutput( 0, thresholdFilter->GetOutput() );
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
void LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::EigenValuesExtractionOn()
{
	m_EigenValuesExtraction = true;
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
void LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::EigenValuesExtractionOff()
{
	m_EigenValuesExtraction = false;
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
void LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::LabelImageOn()
{
	m_LabelImage = true;
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
void LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::LabelImageOff()
{
	m_LabelImage = false;
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::EigenValuesArrayType LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::AbsoluteSortedEigenValuesArrayExtractor
::operator()( const TensorType & A ) const
{
	EigenValuesArrayType eigen;
	A.ComputeEigenValues( eigen );

	std::sort( eigen.Begin(), eigen.End(), Compare() );

	return eigen;
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::VectorType LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::LineDirectionVectorExtractor
::operator()( const TensorType & A ) const
{
	EigenValuesArrayType eigen;
	EigenVectorsMatrixType vectorMatrix;
	A.ComputeEigenAnalysis( eigen, vectorMatrix );

	unsigned int i0 = 0, i1 = 0;
        unsigned int value0 = std::abs( eigen[0] ), value1;
	for( unsigned int i = 1; i < ImageDimension; ++i )
	{
		if( std::abs( eigen[i] ) >= value0 )
		{
			value1 = value0;
			i1 = i0;
			i0 = i;
			value0 = std::abs( eigen[i] );
		}
	}

	CrossHelper< VectorType > cross;
	VectorType v0, v1;
	v0.Fill( NumericTraits< typename VectorType::ValueType >::Zero );
	v1.Fill( NumericTraits< typename VectorType::ValueType >::Zero );

	v0[i0] = 1;
	v1[i1] = 1;


	return cross( v0, v1 ) ;
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::LabelType LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::SignedBasedEigenValuesClassifier
::operator()( const EigenValuesArrayType & A ) const
{
	LabelType output = 0;

	for( unsigned int i = m_DimensionsProcessed; i > 0 ; --i )
	{
		if( A[i] > 0 )
		{
			output |= 1 << ( m_DimensionsProcessed - i );
		}
	}
	return output;
}

template < class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::RealType LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::EigenValuesExtractor
::operator()( const EigenValuesArrayType & A ) const
{
	return A[m_EigenIndex];
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename DataObject::Pointer LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::MakeOutput( unsigned int idx )
{
	DataObject::Pointer output;

	switch( idx )
	{
		case 0:
			output = ( OutputImageType::New() ).GetPointer();
			break;
		case 1:
			output = ( LabelImageType::New() ).GetPointer();
			break;
		case 2:
			output = ( RealImageType::New() ).GetPointer();
			break;
		case 3:
			output = ( RealImageType::New() ).GetPointer();
			break;
		case 4:
			output = ( RealImageType::New() ).GetPointer();
			break;
		case 5:
			output = ( VectorImageType::New() ).GetPointer();
			break;
		case 6:
			output = ( RealImageType::New() ).GetPointer();
			break;
		default:
			std::cerr << " No output " << idx << std::endl;
			output = NULL;
			break;
	}
	return output.GetPointer();
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::OutputImageType* LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::GetBinaryOutput()
{
	return dynamic_cast< OutputImageType* > ( this->ProcessObject::GetOutput( 0 ) );
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::LabelImageType* LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::GetLabelOutput()
{
	return dynamic_cast< LabelImageType* > ( this->ProcessObject::GetOutput( 1 ) );
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::RealImageType* LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::GetEigenValuesOutput( unsigned int i )
{
	return dynamic_cast< RealImageType* > ( this->ProcessObject::GetOutput( 2 + i ) );
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >::VectorImageType* LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::GetLineDirectionOutput()
{
	return dynamic_cast< VectorImageType* > (this->ProcessObject::GetOutput( 5 ) );
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
typename LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::RealImageType* LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::GetVesselnessOutput()
{
	return dynamic_cast< RealImageType* > ( this->ProcessObject::GetOutput( 6 ) );
}

template< class TInputImage, class TOutputImage, class TLabelImage, class TVectorImage >
void LineShapeImageFilter< TInputImage, TOutputImage, TLabelImage, TVectorImage >
::SetRegion( const InputImageRegionType & region )
{
	m_Region = region;
	m_RegionSetByUser = true;
}

} // namespace itk

#endif

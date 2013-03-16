// vtk Interfaces
#include "vtkInterfaces.h"

#include <vtkPoints.h>
#include <vtkPolygon.h>
#include "vtkImageReader2.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCommand.h"
#include "vtkImageData.h"

#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkImageClip.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkTesting.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkStructuredPointsReader.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkToolkits.h>
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtksys/SystemTools.hxx>
#include <vtkImageFourierFilter.h>
#include <vtkJPEGReader.h>
#include <vtkImageActor.h>
#include <vtkSuperquadricSource.h>

#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkPNGReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

// FFTW includes
#include <fftw3.h>

// Math library
#include <math.h>
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "vtkFile is required " << std::endl;
        return EXIT_FAILURE;
    }

    // vtk data file
    std::string filename = argv[1];

    // Rendering window
    vtkSmartPointer<vtkRenderWindow> renderingWindow =
            vtkSmartPointer<vtkRenderWindow>::New();

    // Render interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();

    // Set the interactor to the rendering window
    renderInteractor->SetRenderWindow(renderingWindow);

    // Create a renderer
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();

    // Add the renderer to the render window
    renderingWindow->AddRenderer(renderer);

    // Read the data from a vtk file
    vtkSmartPointer<vtkStructuredPointsReader> dataReader =
            vtkSmartPointer<vtkStructuredPointsReader>::New();

    // Passing the file to the data reader
    dataReader->SetFileName(filename.c_str());

    // Updating the reader
    dataReader->Update();

    // Strucured points
    vtkStructuredPoints* volumeSP = dataReader->GetOutput();

    // Volume attributes
    int volumeExtent[6];
    double volumeSpacing[3];
    double volumeOrigin[3];

    // Get the volume attributes
    volumeSP->GetWholeExtent(volumeExtent);
    volumeSP->GetSpacing(volumeSpacing);
    volumeSP->GetOrigin(volumeOrigin);

    // Volume center
    double volumeCenter[3];
    volumeCenter[0] = volumeOrigin[0] + volumeSpacing[0] * 0.5 * (volumeExtent[0] + volumeExtent[1]);
    volumeCenter[1] = volumeOrigin[1] + volumeSpacing[1] * 0.5 * (volumeExtent[2] + volumeExtent[3]);
    volumeCenter[2] = volumeOrigin[2] + volumeSpacing[2] * 0.5 * (volumeExtent[4] + volumeExtent[5]);

    static double axialElements[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

    static double coronalElements[16] = {
        1, 0, 0, 0,
        0, 0, 1, 0,
        0,-1, 0, 0,
        0, 0, 0, 1 };

    static double sagittalElements[16] = {
        0, 0,-1, 0,
        1, 0, 0, 0,
        0,-1, 0, 0,
        0, 0, 0, 1 };

    static double obliqueElements[16] = {
        1, 0, 0, 0,
        0, 0.866025, -0.5, 0,
        0, 0.5, 0.866025, 0,
        0, 0, 0, 1 };

    // Set the slice orientation
    vtkMatrix4x4 *resliceAxes = vtkMatrix4x4::New();
    resliceAxes->DeepCopy(axialElements);

    // Set the point through which to slice
    resliceAxes->SetElement(0, 3, volumeCenter[0]);
    resliceAxes->SetElement(1, 3, volumeCenter[1]);
    resliceAxes->SetElement(2, 3, volumeCenter[2]);




    // Get the dimensions of the volume for looping
    int* dimensionsVolume = volumeSP->GetDimensions();

    // Printing volume dimensions
    std::cout << "Volume Dimensions " << std::endl;
    std::cout << "X:" << dimensionsVolume[0] << std::endl;
    std::cout << "Y:" << dimensionsVolume[1] << std::endl;
    std::cout << "Z:" << dimensionsVolume[2] << std::endl;


    // Convert the volume into vtkDataArray or vtkDataImage
    const int volDims = dimensionsVolume[0] * dimensionsVolume[1]
            * dimensionsVolume[2];

    // Allocate a spectral volume
    fftw_complex* spectralVolumeData =
            (fftw_complex*) malloc (sizeof(fftw_complex) * volDims);

    // Accessing volume data
    int index = 0;
    for (int i = 0; i < dimensionsVolume[0]; i++)
    {
        for (int j = 0; j < dimensionsVolume[1]; j++)
        {
            for (int k = 0; k < dimensionsVolume[2]; k++)
            {
                // The voxel value
                double voxelValue = (volumeSP->GetScalarComponentAsDouble(i, j, k, 0));

                // Set the values for the spectral volume
                spectralVolumeData[index][0] = voxelValue;
                spectralVolumeData[index][1] = 0;
                index++;
            }
        }
    }

    // Creating fftw plan
    /* @ 3D FFT FORWARD */
    fftw_plan eFFTPlan = fftw_plan_dft_3d(
                dimensionsVolume[0],
                dimensionsVolume[1],
                dimensionsVolume[2],
                spectralVolumeData,
                spectralVolumeData,
                FFTW_FORWARD,
                FFTW_ESTIMATE);

    /* @ executing the FFT plan */
    fftw_execute(eFFTPlan);

#ifdef INVERSE_FFT
    /* @ 3D FFT INVERSE */
    eFFTPlan = fftw_plan_dft_3d(
                dimensionsVolume[0],
                dimensionsVolume[1],
                dimensionsVolume[2],
                spectralVolumeData,
                spectralVolumeData,
                FFTW_BACKWARD,
                FFTW_ESTIMATE);

    /* @ executing the FFT plan */
    fftw_execute(eFFTPlan);
#endif

    // Visualize the real part of the spectrum
    index = 0;
    for (int i = 0; i < dimensionsVolume[0]; i++)
    {
        for (int j = 0; j < dimensionsVolume[1]; j++)
        {
            for (int k = 0; k < dimensionsVolume[2]; k++)
            {
                double voxelValue = sqrt((spectralVolumeData[index][0] * spectralVolumeData[index][0]) +
                                     (spectralVolumeData[index][1] * spectralVolumeData[index][1]))/50;

                volumeSP->SetScalarComponentFromDouble(i, j, k, 0, voxelValue);
                index++;
            }
        }
    }


    // Opacity
    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint(  0.0,  0.0);
    opacityTransferFunction->AddPoint( 90.0,  0.0);
    opacityTransferFunction->AddPoint( 137.9, 0.119);
    opacityTransferFunction->AddPoint(255.0,  0.2);

    // RGB
    vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(0.0,   0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(120.0, 1.0, 1.0, 1.0);
    colorTransferFunction->AddRGBPoint(160.0, 1.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(200.0, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(255.0, 0.0, 1.0, 1.0);

    // Gradient
    vtkPiecewiseFunction *gradientTransferFunction = vtkPiecewiseFunction::New();
    gradientTransferFunction->AddPoint(  0.0, 0);
    gradientTransferFunction->AddPoint(  2.5, 0);
    gradientTransferFunction->AddPoint( 12.7, 1);
    gradientTransferFunction->AddPoint(255.0, 1);

    // Property
    vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    volumeProperty->SetGradientOpacity(gradientTransferFunction);
    volumeProperty->ShadeOff();
    volumeProperty->SetInterpolationTypeToLinear();

    // Composition function
    vtkVolumeRayCastCompositeFunction  *compositeFunction =
            vtkVolumeRayCastCompositeFunction::New();

    // RayCasting Mapper
    vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->SetInputConnection(dataReader->GetOutputPort());

    // Creating a vtk volume
    vtkSmartPointer<vtkVolume> volume =
            vtkSmartPointer<vtkVolume>::New();

    // Add the mapper to the volume
    volume->SetMapper(volumeMapper);

    // Adding the properties of the volume
    volume->SetProperty(volumeProperty);

    // Adding the volume to the renderer
    renderer->AddViewProp(volume);

    // Render the volume
    renderingWindow->Render();

    // Rendering loop
    renderInteractor->Start();

    return EXIT_SUCCESS;
}


//    // Image reader
//    vtkSmartPointer<vtkJPEGReader> jpegReader =
//          vtkSmartPointer<vtkJPEGReader>::New();

//    vtkSmartPointer<vtkJPEGReader>::New();
//    if( !jpegReader->CanReadFile( argv[2] ) )
//    {
//        std::cerr << "Error reading file " << argv[2] << std::endl;
//        return EXIT_FAILURE;
//    }

//    // Get the file
//    jpegReader->SetFileName ( argv[2] );

//    // Update the reader
//    jpegReader->Update();

//    // Image data
//    vtkSmartPointer<vtkImageData> imageData;

//    imageData = jpegReader->GetOutput();

//    // Create an image actor to display the image
//    vtkSmartPointer<vtkImageActor> imageActor =
//            vtkSmartPointer<vtkImageActor>::New();

//    // Set the image data to the actor to display it
//    imageActor->SetInput(imageData);




//    // Get the image data
//    imageData = jpegReader->GetOutput();


//    // Create a renderer to display the image in the background
//    vtkSmartPointer<vtkRenderer> backgroundRenderer =
//            vtkSmartPointer<vtkRenderer>::New();


//    // Create a superquadric
//    vtkSmartPointer<vtkSuperquadricSource> superquadricSource =
//            vtkSmartPointer<vtkSuperquadricSource>::New();
//    superquadricSource->SetPhiRoundness(1.1);
//    superquadricSource->SetThetaRoundness(.2);

//    // Create a mapper
//    vtkSmartPointer<vtkPolyDataMapper> superquadricMapper =
//            vtkSmartPointer<vtkPolyDataMapper>::New();
//    superquadricMapper->SetInputConnection(superquadricSource->GetOutputPort());

//    // Create an actor
//    vtkSmartPointer<vtkActor> superquadricActor =
//            vtkSmartPointer<vtkActor>::New();
//    superquadricActor->SetMapper(superquadricMapper);



//    // Set up the render window and renderers such that there is
//    // a background layer and a foreground layer
//    backgroundRenderer->SetLayer(0);
//    backgroundRenderer->InteractiveOff();






//renderingWindow->AddRenderer(backgroundRenderer);

 // Add actors to the renderers
 // sceneRenderer->AddActor(superquadricActor);
 //backgroundRenderer->AddActor(imageActor);


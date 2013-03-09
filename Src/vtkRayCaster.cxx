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
    vtkVolumeRayCastCompositeFunction  *compositeFunction = vtkVolumeRayCastCompositeFunction::New();

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

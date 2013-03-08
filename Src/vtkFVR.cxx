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

    // Create a transfer function mapping scalar value to opacity values
    vtkSmartPointer<vtkPiecewiseFunction> opacityTF =
            vtkSmartPointer<vtkPiecewiseFunction>::New();

    // Adding a segment to the opacity transfer function
    opacityTF->AddSegment(0, 1.0, 256, 0.6);

    // creating a RGB color transfer function
    vtkSmartPointer<vtkColorTransferFunction> colorTF =
            vtkSmartPointer<vtkColorTransferFunction>::New();

    // Adding 2 sample pointsto the color TF
    colorTF->AddRGBPoint(   0, 1.0, 1.0, 1.0 );
    colorTF->AddRGBPoint( 255, 1.0, 0.0, 1.0 );

    // Property
    vtkSmartPointer<vtkVolumeProperty> property =
            vtkSmartPointer<vtkVolumeProperty>::New();
    // Adding the opacity TF
    property->SetScalarOpacity(opacityTF);

    // Adding the color TF
    property->SetColor(colorTF);

    // Linear interpolation for the volume s
    property->SetInterpolationTypeToLinear();

    // Raycasting Mapper
    vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> raycastingMapper =
            vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();

    // min intensity blending
    raycastingMapper->SetBlendModeToMaximumIntensity();

    // Get the volume from the data reader directly
    raycastingMapper->SetInputConnection(dataReader->GetOutputPort());

    // Creating a vtk volume
    vtkSmartPointer<vtkVolume> volume =
            vtkSmartPointer<vtkVolume>::New();

    // Add the mapper to the volume
    volume->SetMapper(raycastingMapper);

    // Adding the properties of the volume
    volume->SetProperty(property);

    // Adding the volume to the renderer
    renderer->AddViewProp(volume);

    // Render the volume
    renderingWindow->Render();

    // Rendering loop
    renderInteractor->Start();

    return EXIT_SUCCESS;
}

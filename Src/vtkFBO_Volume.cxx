#include <vtkVersion.h>
#if VTK_MAJOR_VERSION == 6
int main(int, char *argv[])
{
    std::cout << argv[0] << " requires VTK 5.10 or earlier. This VTK version is " << vtkVersion::GetVTKVersion() << std::endl;
    return EXIT_SUCCESS;
}
#else
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkGraphicsFactory.h>
#include <vtkImagingFactory.h>

int main(int, char *[])
{
    // Setup offscreen rendering factory
    vtkSmartPointer<vtkGraphicsFactory> offScreenRenderingFactory =
            vtkSmartPointer<vtkGraphicsFactory>::New();

    // Off-screen rendering parameters
    offScreenRenderingFactory->SetOffScreenOnlyMode(1);
    offScreenRenderingFactory->SetUseMesaClasses(1);

    //TODO: Imaging factory
    vtkSmartPointer<vtkImagingFactory> imagingFactory =
            vtkSmartPointer<vtkImagingFactory>::New();
    imagingFactory->SetUseMesaClasses( 1 );

    // Create a sphere in your scene
    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();

    // Create a data mapper and dataActor
    vtkSmartPointer<vtkPolyDataMapper> dataMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    dataMapper->SetInputConnection(sphereSource->GetOutputPort());

    // Actor
    vtkSmartPointer<vtkActor> dataActor =
            vtkSmartPointer<vtkActor>::New();
    dataActor->SetMapper(dataMapper);

    // A renderer and render window
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();

    // Set the renderer to off-screen rendering
    renderWindow->SetOffScreenRendering( 1 );
    renderWindow->AddRenderer(renderer);

    // Add the actors to the scene
    renderer->AddActor(dataActor);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->Render();

    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
            vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->Update();

    vtkSmartPointer<vtkPNGWriter> writer =
            vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName("screenshot.png");
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();

    return EXIT_SUCCESS;
}
#endif

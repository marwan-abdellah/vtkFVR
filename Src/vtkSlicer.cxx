/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: ImageSlicing.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This example shows how to load a 3D image into VTK and then reformat
// that image into a different orientation for viewing.  It uses
// vtkImageReslice for reformatting the image, and uses vtkImageActor
// and vtkInteractorStyleImage to display the image.  This InteractorStyle
// forces the camera to stay perpendicular to the XY plane.
//
// Thanks to David Gobbi of Atamai Inc. for contributing this example.
//

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


// The mouse motion callback, to turn "Slicing" on and off
class vtkImageInteractionCallback : public vtkCommand
{
public:

    static vtkImageInteractionCallback *New() {
        return new vtkImageInteractionCallback; };

    vtkImageInteractionCallback() {
        this->Slicing = 0;
        this->ImageReslice = 0;
        this->Interactor = 0; };

    void SetImageReslice(vtkImageReslice *reslice) {
        this->ImageReslice = reslice; };

    vtkImageReslice *GetImageReslice() {
        return this->ImageReslice; };

    void SetInteractor(vtkRenderWindowInteractor *interactor) {
        this->Interactor = interactor; };

    vtkRenderWindowInteractor *GetInteractor() {
        return this->Interactor; };

    virtual void Execute(vtkObject *, unsigned long event, void *)
    {
        vtkRenderWindowInteractor *interactor = this->GetInteractor();

        int lastPos[2];
        interactor->GetLastEventPosition(lastPos);
        int currPos[2];
        interactor->GetEventPosition(currPos);

        if (event == vtkCommand::LeftButtonPressEvent)
        {
            this->Slicing = 1;
        }
        else if (event == vtkCommand::LeftButtonReleaseEvent)
        {
            this->Slicing = 0;
        }
        else if (event == vtkCommand::MouseMoveEvent)
        {
            if (this->Slicing)
            {
                vtkImageReslice *reslice = this->ImageReslice;

                // Increment slice position by deltaY of mouse
                int deltaY = lastPos[1] - currPos[1];

                reslice->GetOutput()->UpdateInformation();
                double sliceSpacing = reslice->GetOutput()->GetSpacing()[2];
                vtkMatrix4x4 *matrix = reslice->GetResliceAxes();
                // move the center point that we are slicing through
                double point[4];
                double center[4];
                point[0] = 0.0;
                point[1] = 0.0;
                point[2] = sliceSpacing * deltaY;
                point[3] = 1.0;
                matrix->MultiplyPoint(point, center);
                matrix->SetElement(0, 3, center[0]);
                matrix->SetElement(1, 3, center[1]);
                matrix->SetElement(2, 3, center[2]);
                interactor->Render();
            }
            else
            {
                vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(
                            interactor->GetInteractorStyle());
                if (style)
                {
                    style->OnMouseMove();
                }
            }
        }
    };

private:

    // Actions (slicing only, for now)
    int Slicing;

    // Pointer to vtkImageReslice
    vtkImageReslice *ImageReslice;

    // Pointer to the interactor
    vtkRenderWindowInteractor *Interactor;
};

// The program entry point
int main (int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " <DATA_DIR>/headsq/quarter" << endl;
        return 1;
    }

    // vtk data file
    std::string filename = argv[1];

    // Read the data from a vtk file
    vtkSmartPointer<vtkStructuredPointsReader> reader =
            vtkSmartPointer<vtkStructuredPointsReader>::New();

    // Passing the file to the data reader
    reader->SetFileName(filename.c_str());

    // Updating the reader
    reader->Update();

    // Strucutred points
    vtkStructuredPoints* volumeSP = reader->GetOutput();

    int* dimensionsVolume = volumeSP->GetDimensions();

    std::cout << "Volume Dimensions " << std::endl;
    std::cout << "X:" << dimensionsVolume[0] << std::endl;
    std::cout << "Y:" << dimensionsVolume[1] << std::endl;
    std::cout << "Z:" << dimensionsVolume[2] << std::endl;

    int extent[6];
    double spacing[3];
    double origin[3];
    reader->GetOutput()->GetWholeExtent(extent);
    reader->GetOutput()->GetSpacing(spacing);
    reader->GetOutput()->GetOrigin(origin);

    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);

    // Matrices for axial, coronal, sagittal, oblique view orientations
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
                double voxelValue = sqrt((spectralVolumeData[index][0] * spectralVolumeData[index][0]))/20;// +
                                    // (spectralVolumeData[index][1] * spectralVolumeData[index][1]))/50;

                volumeSP->SetScalarComponentFromDouble(i, j, k, 0, voxelValue);
                index++;
            }
        }
    }


    // Set the slice orientation
    vtkMatrix4x4 *resliceAxes = vtkMatrix4x4::New();
    resliceAxes->DeepCopy(axialElements);
    // Set the point through which to slice
    resliceAxes->SetElement(0, 3, center[0]);
    resliceAxes->SetElement(1, 3, center[1]);
    resliceAxes->SetElement(2, 3, center[2]);

    // Extract a slice in the desired orientation
    vtkImageReslice *reslice = vtkImageReslice::New();


    reslice->SetInputConnection(reader->GetOutputPort());

    reslice->SetOutputDimensionality(2);
    reslice->SetResliceAxes(resliceAxes);
    reslice->SetInterpolationModeToLinear();

    // Create a greyscale lookup table
    vtkLookupTable *table = vtkLookupTable::New();
    table->SetRange(0, 2000); // image intensity range
    table->SetValueRange(0.0, 1.0); // from black to white
    table->SetSaturationRange(0.0, 0.0); // no color saturation
    table->SetRampToLinear();
    table->Build();

    // Map the image through the lookup table
    vtkImageMapToColors *color = vtkImageMapToColors::New();
    color->SetLookupTable(table);
    color->SetInputConnection(reslice->GetOutputPort());









//// I have a grayscale image
//std::cout << "**:" << imageData->GetNumberOfScalarComponents() << std::endl;

//int* dims = imageData->GetDimensions();






    // Display the image
    vtkImageActor *actor = vtkImageActor::New();
    actor->SetInput(color->GetOutput());

    //
    vtkImageData *imageData = actor->GetInput();
   // imageData->UpdateData();
    imageData->Update();

    int* sliceDimensions = imageData->GetDimensions();


    std::cout << "XRange:" << *(actor->GetXRange()) << std::endl;
    std::cout << "YRange:" << *(actor->GetYRange()) << std::endl;
    std::cout << "Center:" << (actor->GetLength()) << std::endl;

    std::cout << "Center:" << (actor->GetLength()) << std::endl;

    std::cout << "Slice Dimensions " << std::endl;
    std::cout << "X:" << sliceDimensions[0] << std::endl;
    std::cout << "Y:" << sliceDimensions[1] << std::endl;
    std::cout << "Z:" << sliceDimensions[2] << std::endl;

    double* bounds = actor->GetBounds();
    double xmin = bounds[0];
    double xmax = bounds[1];
    double ymin = bounds[2];
    double ymax = bounds[3];

    std::cout << "xmin:" << xmin << std::endl;
    std::cout << "xmax:" << xmax << std::endl;
    std::cout << "ymin:" << ymin << std::endl;
    std::cout << "ymax:" << ymax << std::endl;

    const int sliceDim = sliceDimensions[0] * sliceDimensions[1];


    // Allocate a spectral volume
    fftw_complex* SpectralSlice =
            (fftw_complex*) malloc (sizeof(fftw_complex) * sliceDim);

index = 0;
    for (int i = 0; i < dimensionsVolume[0]; i++)
    {
        for (int j = 0; j < dimensionsVolume[1]; j++)
        {
            //for (int k = 0; k < dimensionsVolume[2]; k++)
            {
                // std::cout << "Value:" << actor->GetInput()->GetScalarComponentAsFloat(i,j,0,0) << std::endl;

                SpectralSlice[index][0] = actor->GetInput()->GetScalarComponentAsFloat(i,j,0,0);
                SpectralSlice[index][1] = 0;
                index++;


            }
        }
    }

    fftw_plan xFFTPlan = fftw_plan_dft_2d(
                dimensionsVolume[0],
                dimensionsVolume[1],
                SpectralSlice,
                SpectralSlice,
                FFTW_FORWARD,
                FFTW_ESTIMATE);


        fftw_execute(xFFTPlan);

index = 0;
        for (int i = 0; i < dimensionsVolume[0]; i++)
        {
            for (int j = 0; j < dimensionsVolume[1]; j++)
            {
                //for (int k = 0; k < dimensionsVolume[2]; k++)
                {
                    //std::cout << "Value:" << actor->GetInput()->GetScalarComponentAsFloat(i,j,0,0) << std::endl;

                    double val = sqrt((SpectralSlice[index][0] * SpectralSlice[index][0]))/20;


                    actor->GetInput()->SetScalarComponentFromDouble(i,j,0,0,val);
                    index++;


                }
            }
        }

    actor->GetInput()->Update();


    vtkRenderer *renderer = vtkRenderer::New();
    renderer->AddActor(actor);

    vtkRenderWindow *window = vtkRenderWindow::New();
    window->AddRenderer(renderer);

    // Set up the interaction
    vtkInteractorStyleImage *imageStyle = vtkInteractorStyleImage::New();
    vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::New();
    interactor->SetInteractorStyle(imageStyle);
    window->SetInteractor(interactor);
    window->Render();

    vtkImageInteractionCallback *callback = vtkImageInteractionCallback::New();
    callback->SetImageReslice(reslice);
    callback->SetInteractor(interactor);

    imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
    imageStyle->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
    imageStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, callback);

    // Start interaction
    // The Start() method doesn't return until the window is closed by the user
    interactor->Start();

    // Clean up
    callback->Delete();
    interactor->Delete();
    imageStyle->Delete();
    window->Delete();
    renderer->Delete();
    actor->Delete();
    reslice->Delete();
    resliceAxes->Delete();
    color->Delete();
    table->Delete();
    reader->Delete();
}

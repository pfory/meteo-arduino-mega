using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using System.Windows.Controls;

namespace SolarSystem
{
    public partial class Window1 : System.Windows.Window
    {

        public Window1()
        {
            InitializeComponent();

            Model3DGroup models = new Model3DGroup();

            Model3D rectangle1 = CreateRectangle(
                    new Point3D(0, 0, 0),
                    new Point3D(1, 0, 0),
                    new Point3D(1, 1, 0),
                    new Point3D(0, 1, 0),
                    Brushes.Red);

            Model3D rectangle2 = CreateRectangle(
                    new Point3D(0.5, 0.5, -1),
                    new Point3D(1.5, 0.5, -1),
                    new Point3D(1.5, 1.5, -1),
                    new Point3D(0.5, 1.5, -1),
                    Brushes.Green);

            models.Children.Add(rectangle1);
            models.Children.Add(rectangle2);

            ModelVisual3D visual = new ModelVisual3D();
            visual.Content = models;

            myViewport3D.Children.Add(visual);
        }

        public GeometryModel3D CreateRectangle(
            Point3D point1, Point3D point2,
            Point3D point3, Point3D point4,
            Brush brush)
        {
            MeshGeometry3D mesh = new MeshGeometry3D();
            mesh.Positions.Add(point1);
            mesh.Positions.Add(point2);
            mesh.Positions.Add(point3);
            mesh.Positions.Add(point4);

            mesh.TriangleIndices.Add(0);
            mesh.TriangleIndices.Add(1);
            mesh.TriangleIndices.Add(2);

            mesh.TriangleIndices.Add(0);
            mesh.TriangleIndices.Add(2);
            mesh.TriangleIndices.Add(3);

            return new GeometryModel3D(mesh,
                new DiffuseMaterial(brush));
        }
    }
}
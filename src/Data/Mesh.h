#pragma once
/*******************************************************************************

  Copyright (C) 2022 Andrew Gilbert

  This file is part of IQmol, a free molecular visualization program. See
  <http://iqmol.org> for more details.

  IQmol is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  IQmol is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along
  with IQmol.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************************/

#include "Data/DataList.h"
#include "Math/Function.h"
#include "OpenMesh/Core/IO/Options.hh"
#include "OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh"
#include <QPair>
#include "QGLViewer/vec.h"


namespace IQmol {

namespace Layer {
   class Surface;
}

namespace Data {

   struct MeshTraits : public OpenMesh::DefaultTraits {
      VertexAttributes(OpenMesh::Attributes::Status);
      FaceAttributes(OpenMesh::Attributes::Status);
      EdgeAttributes(OpenMesh::Attributes::Status);
   };

   typedef OpenMesh::TriMesh_ArrayKernelT<MeshTraits> OMMesh;

   /// Wrapper around the OpenMesh TriMesh class to provide serialization functionality.
   class Mesh : public Base {

      friend class IQmol::Layer::Surface;

    public:
         typedef OMMesh::VertexHandle  Vertex;
         typedef OMMesh::FaceHandle    Face;
         typedef OMMesh::Point         Point;
         typedef OMMesh::Normal        Normal;
         typedef QPair<Vertex, Vertex> Edge;
         typedef QMap<Edge, Vertex>    NewVertexMap;
         typedef std::function<double (Vertex const&)> VertexFunction;

      public:
         enum Property 
         { 
            FaceNormals, 
            VertexNormals,
            FaceCentroids, 
            ScalarField, 
            MeshIndex, 
            IndexField 
        };

         Mesh();
         Mesh(Mesh const& that);

         Type::ID typeID() const { return Type::Mesh; }
 
         Mesh& operator+=(Mesh const&);
         Mesh& operator=(Mesh const& that);

         Vertex addVertex(OMMesh::Point const p);
         Vertex addVertex(double const x, double const y, double const z);
         Face   addFace(Vertex const& v0, Vertex const& v1, Vertex const& v2);

         void setNormal(Vertex const& handle, double dx, double dy, double dz);
         void setNormal(Vertex const& handle, Normal const& normal);
         void setPoint(Vertex const& handle, Point const& p);

         Point  const& faceCentroid(Face const& face) const;
         Point  const& vertex(Vertex const& vertex) const;
         Normal const& normal(Face const& face) const;
         Normal const& normal(Vertex const& vertex) const;

         // Iterator convenience functions
         OMMesh::VertexIter vbegin() { return m_omMesh.vertices_begin(); }
         OMMesh::VertexIter vend()   { return m_omMesh.vertices_end(); }
         OMMesh::ConstVertexIter vbegin() const { return m_omMesh.vertices_begin(); }
         OMMesh::ConstVertexIter vend()   const { return m_omMesh.vertices_end(); }

         OMMesh::FaceIter fbegin() { return m_omMesh.faces_begin(); }
         OMMesh::FaceIter fend()   { return m_omMesh.faces_end(); }
         OMMesh::ConstFaceIter fbegin() const { return m_omMesh.faces_begin(); }
         OMMesh::ConstFaceIter fend()   const { return m_omMesh.faces_end(); }

         bool hasProperty(Property const property) const;
         bool requestProperty(Property const property);

		 /// Deletes the specified property.  Note that nothing is done for 
         /// FaceNormals/Centroids and VertexNormals as they are always required. 
         void deleteProperty(Property const property);

         bool setIndexField(Vertex const& vertex, int const);
         int indexFieldValue(Vertex const& vertex) const;

         bool computeScalarField(Function3D const&);
         bool computeScalarField(VertexFunction const&);
         void getScalarFieldRange(double& min, double& max);

         double scalarFieldValue(OMMesh::VertexHandle const& vertex) const;

		 /// Sets the value of the MeshIndex property for each face of the mesh.
		 /// This is used when creating a composite Mesh (using +=) and the 
         /// identity of the individual meshes is still required.
         bool setMeshIndex(int const index);

         int meshIndex(Face const& face) const;

         void writeToFile();

         /// Applies a non-reversable clipping plane to the mesh
         void clip(qglviewer::Vec const& normal, qglviewer::Vec const& pointOnPlane);

         double surfaceArea() const;

         void dump() const;

         /// Computes the face centroids and updates the face normals
         bool computeFaceNormals();

         bool computeVertexNormals();

         OMMesh const& data() const { return m_omMesh; } 
         OMMesh& data() { return m_omMesh; } 


      private:
		 /// Specifies the format that the mesh is stored in the archive.
         static std::string const s_archiveFormat;

		 /// String identifiers for custom properties
         static std::string const s_scalarFieldString;
         static std::string const s_indexFieldString;
         static std::string const s_meshIndexString;
         static std::string const s_faceCentroidsString;

         /// Threshold used to determine when vertices refer to the same point.
         static double const s_thresh;

         void copy(Mesh const& that);

         // This requests the properties that every mesh must have, all the time
         void requestDefaultProperties();

		 /// Returns the location of the point of intersection between the
		 /// plane (n, p0) and the AB line.  d is loaded with the distance 
         /// from A to the intersection. 
         Point linePlaneIntersection(Normal const& n, Point const& p0, Point const& A, 
            Point const& B, double& d);

		 /// Returns the vertex corresponding to the intersection between the
		 /// AB line and the plane.  If the vertex does not exist (as a value 
         /// in the map), it is created.
         Vertex createLinePlaneIntersection(Normal const& n, Point const& p0,  
            Vertex const& A, Vertex const& B, NewVertexMap& map);

         /// Moves vertex A to the intersection of the plane (n, p0) with AB
         void moveToLinePlaneIntersection(Normal const& n, Point const& p0, Vertex& A, 
            Vertex const& B);

         void removeDisconnectedVertices();
             
         /// This is the key data structure holding the Mesh information.
         OMMesh m_omMesh;

         /// Property handle for the face centroids used for plotting face normals
         OpenMesh::FPropHandleT<OMMesh::Point>  m_faceCentroidsHandle;

         /// Property handle for a scalar quantitiy, such as the ESP.
         OpenMesh::VPropHandleT<double> m_scalarFieldHandle;

         /// Property handle for an indexed quantitiy, such as the residue.
         OpenMesh::VPropHandleT<int> m_indexFieldHandle;

		 /// Property handle for atom indices.  This is used for coloring the
		 /// mesh based on atomic properties such as element number or charge.
         OpenMesh::FPropHandleT<int>    m_meshIndexHandle;
   };

   inline double NullFunction(Mesh::Vertex const&) { return 0.0; }

   class MeshList : public List<Mesh> { 
      public:
         Type::ID typeID() const { return Type::MeshList; }
   }; 

} } // end namespace IQmol::Data

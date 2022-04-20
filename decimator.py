import pymeshlab as ml
from sys import argv

if __name__ == "__main__":
    if len(argv) < 3 or len(argv) > 4:
        print("Usage: ")
        print("     python decimator.py <input_mesh_file> <output_mesh_file> <number_of_faces>")
        print("     if no output file is specified, the output will be written under the name <input_mesh_file>_decimated.obj")
        exit(1)
    elif len(argv) == 3:
        numFaces = int(argv[2])
        inputFile = argv[1]
        outputFile = argv[1][:-4] + "_decimated.obj"
    elif len(argv) == 4:
        numFaces = int(argv[3])
        inputFile = argv[1]
        outputFile = argv[2]

    ms = ml.MeshSet()
    ms.load_new_mesh(inputFile)
    m = ms.current_mesh()
    print('input mesh has', m.vertex_number(), 'vertex and', m.face_number(), 'faces')

    #Simplify the mesh. Only first simplification will be agressive
    while (ms.current_mesh().face_number() > numFaces):
        ms.apply_filter('simplification_quadric_edge_collapse_decimation', targetfacenum=numFaces, preservenormal=True)
        print("Decimated to", numFaces, "faces mesh has", ms.current_mesh().vertex_number(), "vertex")
        numFaces = numFaces - (ms.current_mesh().face_number() - numFaces)

    m = ms.current_mesh()
    print('output mesh has', m.vertex_number(), 'vertex and', m.face_number(), 'faces')
    ms.save_current_mesh(outputFile)
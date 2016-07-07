function parse_3d_file(filename, s)
{
	switch (filename.split('.').pop().toLowerCase())
	{
		case "stl":
            return parse_stl_ascii(s);
		default:
			return "Unknown file type";
	}
}

function parse_stl_ascii (stl_string)
{
	try
	{	
		var vertices=[];
		var faces=[];
		var vert_hash = {};
        var f1,f2,f3;

		stl_string = stl_string.replace(/\r/, "\n");
		stl_string = stl_string.replace(/^solid[^\n]*/, "");
		stl_string = stl_string.replace(/\n/g, " ");
		stl_string = stl_string.replace(/facet normal /g,"");
		stl_string = stl_string.replace(/outer loop/g,"");  
		stl_string = stl_string.replace(/vertex /g,"");
		stl_string = stl_string.replace(/endloop/g,"");
		stl_string = stl_string.replace(/endfacet/g,"");
		stl_string = stl_string.replace(/endsolid[^\n]*/, "");
		stl_string = stl_string.replace(/\s+/g, " ");
		stl_string = stl_string.replace(/^\s+/, "");
		
		var facet_count = 0;
		var block_start = 0;
		var vertex;
		var vertexIndex;
		var points = stl_string.split(" ");
		var face_indices=[];
		var len=points.length/12-1;
	    
		for (var i=0; i<len; i++)
		{
			face_indices = [];
			for (var x=0; x<3; x++)
			{
				f1=parseFloat(points[block_start+x*3+3]);
				f2=parseFloat(points[block_start+x*3+4]);
				f3=parseFloat(points[block_start+x*3+5]);
				vertexIndex = vert_hash[ [f1,f2,f3] ];
                if (vertexIndex == null)
				{
					vertexIndex = vertices.length;
					vertices.push(new THREE.Vector3(f1,f2,f3));
					vert_hash[ [f1,f2,f3] ] = vertexIndex;
				}

				face_indices.push(vertexIndex);
			}
			faces.push(new THREE.Face3(face_indices[0],face_indices[1],face_indices[2]));
		    
			block_start = block_start + 12;
		}

		return ({vertices:vertices, faces:faces, colors:false});
	}
	catch(err)
	{
        console.log(err.message);
        return "Cannot parse file";
	}
	
}

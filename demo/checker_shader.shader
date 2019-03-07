shader_type spatial;


void fragment() {
	float checkSize = 200.0;
	float fmodResult = mod(floor(checkSize * UV.x) + floor(checkSize * UV.y), 2.0);
	float fin = max(sign(fmodResult), 0.0);
	ALBEDO = vec3(fin, fin, fin); // use red for material albedo
}
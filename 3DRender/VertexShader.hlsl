// To be able to return multiple values in the main function we need a struct to be able to hold those values.
struct VSOut {
	// The order of these matter. In the pixel shader the input parameters must be in the same order as these
	float3 color: COLOR;
	float4 pos : SV_POSITION;
};

cbuffer ConstantBuffer {
	matrix transform;
};

VSOut main(float2 pos : POSITION, float3 color : COLOR)
{
	VSOut vso;
	// Multiplication in vertex shaders have the matrix on the right and the vector on the left. However, we want it to be row major instead of col major
	// a quick way to do that is to multiply by the matrix on the left and the vector on the right.
	vso.pos = mul(transform, float4(pos.x, pos.y, 0.0f, 1.0f));
	vso.color = color;
	return vso;
}
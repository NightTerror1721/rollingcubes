openlib "base"
openlib "Rollingcube.geometry"

print("Hello World")

vec = vec3(0, 5, 3)
vec = vec + -(vec3(vec) * 3)

m = mat4.identity()
val = m * vec4(vec, 1)

m[2][0] = 42

print(vec)
print(val)
print(m)
print(m[1])

m = mat4.ortho(0, 1920, 0, 1080, -1, 1)
print(m)

print(pairs(_ENV))

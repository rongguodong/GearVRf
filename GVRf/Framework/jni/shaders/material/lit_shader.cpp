/* Copyright 2015 Samsung Electronics Co., LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/***************************************************************************
 * Renders a texture with light.
 ***************************************************************************/

#include "lit_shader.h"

#include "gl/gl_program.h"
#include "objects/material.h"
#include "objects/mesh.h"
#include "objects/components/render_data.h"
#include "objects/textures/texture.h"
#include "util/gvr_gl.h"

#include "util/gvr_log.h"

namespace gvr {
static const char VERTEX_SHADER[] =
		"attribute vec4 a_position;\n"
				"attribute vec4 a_tex_coord;\n"
				"attribute vec3 a_normal;\n"
				"uniform mat4 u_mv;\n"
				"uniform mat4 u_mv_it;\n"
				"uniform mat4 u_mvp;\n"
				"varying vec3 v_tex_coord;\n"
				"varying vec3 v_viewspace_normal;\n"
				"varying vec3 v_viewspace_light_direction;\n"
				"\n"
				"void main() {\n"
				"  vec4 v_viewspace_position_vec4 = u_mv * a_position;\n"
				"  vec3 v_viewspace_position = v_viewspace_position_vec4.xyz / v_viewspace_position_vec4.w;\n"
				"  v_viewspace_light_direction = vec3(100.0f, 100.0f, 100.0f) - v_viewspace_position;\n"
				"  v_viewspace_normal = (u_mv_it * vec4(a_normal, 1.0f)).xyz;\n"
				"  v_tex_coord = a_tex_coord;\n"
				"  gl_Position = u_mvp * a_position;\n"
				"}\n";

static const char FRAGMENT_SHADER[] =
		"precision highp float;\n"
				"uniform sampler2D u_texture;\n"
				"uniform vec3 u_color;\n"
				"uniform float u_opacity;\n"
				"varying vec3 v_tex_coord;\n"
				"varying vec3 v_viewspace_normal;\n"
				"varying vec3 v_viewspace_light_direction;\n"
				"\n"
				"void main()\n"
				"{\n"
				"  vec4 ambientColor = vec4(0.2f, 0.2f, 0.2f, 1.0f);"
				"  vec4 deffuseColor = vec4(1.0ff, 1.0f, 1.0f, 1.0f);"
				"  vec4 specularColor = vec4(0.2f, 0.2f, 0.2f, 1.0f);"
				"  // Dot product gives us diffuse intensity\n"
				"  float diffuse = max(0.0, dot(normalize(v_viewspace_normal), normalize(v_viewspace_light_direction)));\n"
				"\n"
				"  // Multiply intensity by diffuse color, force alpha to 1.0\n"
				"  vec4 color = diffuse * diffuseColor;\n"
				"\n"
				"  // Add in ambient light\n"
				"  color += ambientColor;\n"
				"\n"
				"  // Modulate in the texture\n"
				"  color *= texture2D(u_texture, v_tex_coord.xyz);\n"
				"\n"
				"  // Specular Light\n"
				"  vec3 reflection = normalize(reflect(-normalize(v_viewspace_light_direction), normalize(v_viewspace_normal)));\n"
				"  float specular = max(0.0, dot(normalize(v_viewspace_normal), reflection));\n"
				"  if(diffuse != 0) {\n"
				"    color += pow(specular, 128.0) * specularColor;\n"
				"  }\n"
				"\n"
				"  gl_FragColor = vec4(color.r * u_color.r * u_opacity, color.g * u_color.g * u_opacity, color.b * u_color.b * u_opacity, color.a * u_opacity);\n"
				"  gl_FragColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);\n"
				"}\n";

LitShader::LitShader() :
		program_(0), a_position_(0), a_tex_coord_(0), a_normal_(0), u_mv_(0), u_mv_it_(
				0), u_mvp_(0), u_texture_(0), u_color_(0), u_opacity_(0) {
	program_ = new GLProgram(VERTEX_SHADER, FRAGMENT_SHADER);
	LOGE("TESTTEST program_: %d.", program_->id());
	a_position_ = glGetAttribLocation(program_->id(), "a_position");
	a_tex_coord_ = glGetAttribLocation(program_->id(), "a_tex_coord");
	a_normal_ = glGetAttribLocation(program_->id(), "a_normal");
	u_mv_ = glGetUniformLocation(program_->id(), "u_mv");
	u_mv_it_ = glGetUniformLocation(program_->id(), "u_mv_it");
	u_mvp_ = glGetUniformLocation(program_->id(), "u_mvp");
	u_texture_ = glGetUniformLocation(program_->id(), "u_texture");
	u_color_ = glGetUniformLocation(program_->id(), "u_color");
	u_opacity_ = glGetUniformLocation(program_->id(), "u_opacity");
}

LitShader::~LitShader() {
	if (program_ != 0) {
		recycle();
	}
}

void LitShader::recycle() {
	delete program_;
	program_ = 0;
}

void LitShader::render(const glm::mat4& mv_matrix,
		const glm::mat4& mv_it_matrix, const glm::mat4& mvp_matrix,
		RenderData* render_data) {
	Mesh* mesh = render_data->mesh();
	Texture* texture = render_data->material()->getTexture("main_texture");
	glm::vec3 color = render_data->material()->getVec3("color");
	float opacity = render_data->material()->getFloat("opacity");

	if (texture->getTarget() != GL_TEXTURE_2D) {
		std::string error = "CubemapShader::render : texture with wrong target";
		throw error;
	}

	LOGE("TESTTEST pos0.");
	LOGE("TESTTEST a_position_: %d.", a_position_);

#if _GVRF_USE_GLES3_
	mesh->setVertexLoc(a_position_);
	LOGE("TESTTEST pos1.");
	mesh->setTexCoordLoc(a_tex_coord_);
	LOGE("TESTTEST pos2.");
	mesh->setNormalLoc(a_normal_);
	LOGE("TESTTEST pos3.");
	mesh->generateVAO();


	glUseProgram(program_->id());

	glUniformMatrix4fv(u_mv_, 1, GL_FALSE, glm::value_ptr(mv_matrix));
	glUniformMatrix4fv(u_mv_it_, 1, GL_FALSE, glm::value_ptr(mv_it_matrix));
	glUniformMatrix4fv(u_mvp_, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(texture->getTarget(), texture->getId());
	glUniform1i(u_texture_, 0);
	glUniform3f(u_color_, color.r, color.g, color.b);
	glUniform1f(u_opacity_, opacity);

	glBindVertexArray(mesh->getVAOId());
	glDrawElements(GL_TRIANGLES, mesh->triangles().size(), GL_UNSIGNED_SHORT,
			0);
	glBindVertexArray(0);

	LOGE("TESTTEST pos4.");

#else
	glUseProgram(program_->id());

	glVertexAttribPointer(a_position_, 3, GL_FLOAT, GL_FALSE, 0,
			mesh->vertices().data());
	glEnableVertexAttribArray(a_position_);

	glUniformMatrix4fv(u_mv_, 1, GL_FALSE, glm::value_ptr(mv_matrix));
	glUniformMatrix4fv(u_mv_it_, 1, GL_FALSE, glm::value_ptr(mv_it_matrix));
	glUniformMatrix4fv(u_mvp_, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

	glActiveTexture (GL_TEXTURE0);
	glBindTexture(texture->getTarget(), texture->getId());
	glUniform1i(u_texture_, 0);

	glUniform3f(u_color_, color.r, color.g, color.b);

	glUniform1f(u_opacity_, opacity);

	glDrawElements(GL_TRIANGLES, mesh->triangles().size(), GL_UNSIGNED_SHORT,
			mesh->triangles().data());
#endif

	checkGlError("LitShader::render");
}

}
;

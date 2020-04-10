#pragma once
#include "../../source-sdk/misc/color.hpp"

class i_render_view {
private:
	virtual void __pad0();
	virtual void __pad1();
	virtual void __pad2();
	virtual void __pad3();

public:
	virtual void set_blend(float blend) = 0;
	virtual float get_blend(void) = 0;

	virtual void modulate_color(float const* blend) = 0;
	virtual void get_color_modulation(float* blend) = 0;
};
class c_viewsetup
{
public:
	int			x, x_old;
	int			y, y_old;
	int			width, width_old;
	int			height, height_old;
	bool		m_bOrtho;
	float		m_OrthoLeft;
	float		m_OrthoTop;
	float		m_OrthoRight;
	float		m_OrthoBottom;
	bool		m_bCustomViewMatrix;
	matrix_t	m_matCustomViewMatrix;
	char		pad_0x68[0x48];
	float		fov;
	float		fovViewmodel;
	vec3_t		origin;
	vec3_t		angles;
	float		zNear;
	float		zFar;
	float		zNearViewmodel;
	float		zFarViewmodel;
	float		m_flAspectRatio;
	float		m_flNearBlurDepth;
	float		m_flNearFocusDepth;
	float		m_flFarFocusDepth;
	float		m_flFarBlurDepth;
	float		m_flNearBlurRadius;
	float		m_flFarBlurRadius;
	int			m_nDoFQuality;
	int			m_nMotionBlurMode;
	float		m_flShutterTime;
	vec3_t		m_vShutterOpenPosition;
	vec3_t		m_shutterOpenAngles;
	vec3_t		m_vShutterClosePosition;
	vec3_t		m_shutterCloseAngles;
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;
	int			m_EdgeBlur;
};
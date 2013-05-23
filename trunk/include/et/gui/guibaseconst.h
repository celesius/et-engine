#pragma once

namespace et
{
	namespace gui
	{
		enum ElementState
		{
			ElementState_Default,
			ElementState_Hovered,
			ElementState_Pressed,
			ElementState_Selected,
			ElementState_SelectedHovered,
			ElementState_SelectedPressed,
			ElementState_max
		};

		enum AnimationFlags
		{
			AnimationFlag_None = 0x0,
			AnimationFlag_Fade = 0x01,
			AnimationFlag_FromLeft = 0x02,
			AnimationFlag_FromRight = 0x04,
			AnimationFlag_FromTop = 0x08,
			AnimationFlag_FromBottom = 0x10
		};

		enum ElementFlags
		{
			ElementFlag_RequiresKeyboard = 0x0001,
			ElementFlag_Dragable = 0x0002,
			ElementFlag_TransparentForPointer = 0x0004,
			ElementFlag_RenderTopmost = 0x0008,
			ElementFlag_HandlesChildEvents = 0x0010,
			ElementFlag_ClipToBounds = 0x0020,
		};

		enum ElementAnimatedPropery
		{
			ElementAnimatedProperty_None,
			ElementAnimatedProperty_Angle,
			ElementAnimatedProperty_Scale,
			ElementAnimatedProperty_Color,
			ElementAnimatedProperty_Frame,
			ElementAnimatedProperty_max
		};

		enum ElementLayoutMode
		{
			ElementLayoutMode_Absolute,
			ElementLayoutMode_RelativeToParent,
			ElementLayoutMode_RelativeToContext
		};

		enum RenderLayer 
		{
			RenderLayer_Layer0,
			RenderLayer_Layer1,
			RenderLayer_max
		};

		enum ElementAlignment
		{
			ElementAlignment_Near,
			ElementAlignment_Center,
			ElementAlignment_Far,
			ElementAlignment_max,
		};
	}
}
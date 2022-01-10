/*=============================================================================*/
// Copyright 2021 Elite Engine 2.0
// Authors: Thomas Goussaert
/*=============================================================================*/
// ECamera.h: Base Camera Implementation with movement
/*=============================================================================*/

#pragma once
#include "EMath.h"

namespace Elite
{
	class Camera
	{
	public:

		Camera
		(
			const int screenWidth, 
			const int screenHeight, 
			const FPoint3& position = { 0.f, 0.f, -10.f }, 
			const FVector3& viewForward = { 0.f, 0.f, 1.f }, 
			float fovAngle = 60.f, 
			float nearClip = 0.1f, 
			float farClip = 100.f
		);
		~Camera() = default;

		Camera(const Camera&) = delete;
		Camera(Camera&&) noexcept = delete;
		Camera& operator=(const Camera&) = delete;
		Camera& operator=(Camera&&) noexcept = delete;

		void Update(float elapsedSec);

		const FMatrix4& GetLHWorldToView() const { return m_LHWorldToView; }
		const FMatrix4& GetLHViewToWorld() const { return m_LHViewToWorld; }
		const FMatrix4& GetLHProjection() const { return  m_LHProjection;  }

		const FMatrix4& GetRHWorldToView() const { return m_RHWorldToView; }
		const FMatrix4& GetRHViewToWorld() const { return m_RHViewToWorld; }
		const FMatrix4& GetRHProjection() const { return  m_RHProjection; }

		int GetScreenWidth() const { return m_Width; }
		int GetScreenHeight() const { return m_Height; }
		float GetAspectRatio() const { return m_AspectRatio; }
		float GetFov() const { return m_Fov; }

	private:
		void CalculateLookAt();
		void CalculateProjection();

		const int m_Width{};
		const int m_Height{};
		const float m_AspectRatio{};
		const float m_Fov{};

		const float m_KeyboardMoveSensitivity{ 1.f };
		const float m_KeyboardMoveMultiplier{ 10.f };
		const float m_MouseRotationSensitivity{ .1f };
		const float m_MouseMoveSensitivity{ 2.f };

		FPoint2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
		FPoint3 m_RelativeTranslation{};

		FPoint3 m_Position{};
		const FVector3 m_ViewForward{};

		FMatrix4 m_LHWorldToView{};
		FMatrix4 m_LHViewToWorld{};
		FMatrix4 m_LHProjection{};

		FMatrix4 m_RHWorldToView{};
		FMatrix4 m_RHViewToWorld{};
		FMatrix4 m_RHProjection{};

		const float m_NearClipPlane;
		const float m_FarClipPlane;

	};
}

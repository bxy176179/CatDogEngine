#include "SceneWorld.h"

#include "Log/Log.h"
#include "Path/Path.h"
#include "U_BaseSlot.sh"

#if (defined DDGI_SDK_PATH && defined NDEBUG)
	#define ENABLE_DDGI_SDK
#endif

#ifdef ENABLE_DDGI_SDK
#include "ddgi_sdk.h"
#endif

#include <vector>
#include <string>

namespace engine
{

SceneWorld::SceneWorld()
{
	m_pSceneDatabase = std::make_unique<cd::SceneDatabase>();

	m_pWorld = std::make_unique<engine::World>();

	// To add a new component : 2. Init component type here.
	m_pAnimationComponentStorage = m_pWorld->Register<engine::AnimationComponent>();
	m_pCameraComponentStorage = m_pWorld->Register<engine::CameraComponent>();
	m_pCollisionMeshComponentStorage = m_pWorld->Register<engine::CollisionMeshComponent>();
	m_pDDGIComponentStorage = m_pWorld->Register<engine::DDGIComponent>();
	m_pHierarchyComponentStorage = m_pWorld->Register<engine::HierarchyComponent>();
	m_pLightComponentStorage = m_pWorld->Register<engine::LightComponent>();
	m_pMaterialComponentStorage = m_pWorld->Register<engine::MaterialComponent>();
	m_pNameComponentStorage = m_pWorld->Register<engine::NameComponent>();
	m_pSkyComponentStorage = m_pWorld->Register<engine::SkyComponent>();
	m_pStaticMeshComponentStorage = m_pWorld->Register<engine::StaticMeshComponent>();
	m_pTransformComponentStorage = m_pWorld->Register<engine::TransformComponent>();

	CreatePBRMaterialType();
	CreateAnimationMaterialType();
	CreateTerrainMaterialType();
	CreateDDGIMaterialType();
}

void SceneWorld::CreatePBRMaterialType()
{
	m_pPBRMaterialType = std::make_unique<MaterialType>();
	m_pPBRMaterialType->SetMaterialName("CD_PBR");

	ShaderSchema shaderSchema(Path::GetBuiltinShaderInputPath("shaders/vs_PBR"), Path::GetBuiltinShaderInputPath("shaders/fs_PBR"));
	shaderSchema.RegisterUberOption(Uber::ALBEDO_MAP);
	shaderSchema.RegisterUberOption(Uber::NORMAL_MAP);
	shaderSchema.RegisterUberOption(Uber::ORM_MAP);
	shaderSchema.RegisterUberOption(Uber::EMISSIVE_MAP);
	shaderSchema.RegisterUberOption(Uber::IBL);
	shaderSchema.RegisterUberOption(Uber::ATM);
	m_pPBRMaterialType->SetShaderSchema(cd::MoveTemp(shaderSchema));

	cd::VertexFormat pbrVertexFormat;
	pbrVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);
	pbrVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	pbrVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	pbrVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	m_pPBRMaterialType->SetRequiredVertexFormat(cd::MoveTemp(pbrVertexFormat));

	// Slot index should align to shader codes.
	// We want basic PBR materials to be flexible.
	m_pPBRMaterialType->AddOptionalTextureType(cd::MaterialTextureType::BaseColor, ALBEDO_MAP_SLOT);
	m_pPBRMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Normal, NORMAL_MAP_SLOT);
	m_pPBRMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Occlusion, ORM_MAP_SLOT);
	m_pPBRMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Roughness, ORM_MAP_SLOT);
	m_pPBRMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Metallic, ORM_MAP_SLOT);
	m_pPBRMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Emissive, EMISSIVE_MAP_SLOT);
}

void SceneWorld::CreateAnimationMaterialType()
{
	m_pAnimationMaterialType = std::make_unique<MaterialType>();
	m_pAnimationMaterialType->SetMaterialName("CD_Animation");

	ShaderSchema shaderSchema(Path::GetBuiltinShaderInputPath("shaders/vs_animation"), Path::GetBuiltinShaderInputPath("shaders/fs_animation"));
	m_pAnimationMaterialType->SetShaderSchema(cd::MoveTemp(shaderSchema));

	cd::VertexFormat animationVertexFormat;
	animationVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);
	//animationVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	animationVertexFormat.AddAttributeLayout(cd::VertexAttributeType::BoneIndex, cd::AttributeValueType::Int16, 4U);
	animationVertexFormat.AddAttributeLayout(cd::VertexAttributeType::BoneWeight, cd::AttributeValueType::Float, 4U);
	m_pAnimationMaterialType->SetRequiredVertexFormat(cd::MoveTemp(animationVertexFormat));
}

void SceneWorld::CreateTerrainMaterialType()
{
	m_pTerrainMaterialType = std::make_unique<MaterialType>();
	m_pTerrainMaterialType->SetMaterialName("CD_Terrain");

	ShaderSchema shaderSchema(Path::GetBuiltinShaderInputPath("shaders/vs_terrain"), Path::GetBuiltinShaderInputPath("shaders/fs_terrain"));
	shaderSchema.RegisterUberOption(Uber::DEFAULT);
	m_pTerrainMaterialType->SetShaderSchema(cd::MoveTemp(shaderSchema));

	cd::VertexFormat terrainVertexFormat;
	terrainVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);
	terrainVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	m_pTerrainMaterialType->SetRequiredVertexFormat(cd::MoveTemp(terrainVertexFormat));

	m_pTerrainMaterialType->AddRequiredTextureType(cd::MaterialTextureType::Elevation, 1);
	m_pTerrainMaterialType->AddOptionalTextureType(cd::MaterialTextureType::AlphaMap, 2);
}

void SceneWorld::CreateDDGIMaterialType()
{
	m_pDDGIMaterialType = std::make_unique<MaterialType>();
	m_pDDGIMaterialType->SetMaterialName("CD_DDGI");

	ShaderSchema shaderSchema(Path::GetBuiltinShaderInputPath("shaders/vs_DDGI"), Path::GetBuiltinShaderInputPath("shaders/fs_DDGI"));
	shaderSchema.RegisterUberOption(Uber::ALBEDO_MAP);
	shaderSchema.RegisterUberOption(Uber::NORMAL_MAP);
	shaderSchema.RegisterUberOption(Uber::ORM_MAP);
	shaderSchema.RegisterUberOption(Uber::EMISSIVE_MAP);
	// shaderSchema.RegisterUberOption(Uber::DDGI);
	m_pDDGIMaterialType->SetShaderSchema(cd::MoveTemp(shaderSchema));

	cd::VertexFormat ddgiVertexFormat;
	ddgiVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);
	ddgiVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	ddgiVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	ddgiVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	m_pDDGIMaterialType->SetRequiredVertexFormat(cd::MoveTemp(ddgiVertexFormat));

	m_pDDGIMaterialType->AddOptionalTextureType(cd::MaterialTextureType::BaseColor, ALBEDO_MAP_SLOT);
	m_pDDGIMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Normal, NORMAL_MAP_SLOT);
	m_pDDGIMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Occlusion, ORM_MAP_SLOT);
	m_pDDGIMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Roughness, ORM_MAP_SLOT);
	m_pDDGIMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Metallic, ORM_MAP_SLOT);
	m_pDDGIMaterialType->AddOptionalTextureType(cd::MaterialTextureType::Emissive, EMISSIVE_MAP_SLOT);
}

void SceneWorld::SetSelectedEntity(engine::Entity entity)
{
	CD_TRACE("Select entity : {0}", entity);
	m_selectedEntity = entity;
}

void SceneWorld::SetMainCameraEntity(engine::Entity entity)
{
	CD_TRACE("Setup main camera entity : {0}", entity);
	m_mainCameraEntity = entity;
}

void SceneWorld::SetDDGIEntity(engine::Entity entity)
{
	CD_TRACE("Setup DDGI entity : {0}", entity);
	m_ddgiEntity = entity;
}

void SceneWorld::SetSkyEntity(engine::Entity entity)
{
	CD_TRACE("Setup Sky entity : {0}", entity);
	m_skyEntity = entity;
}

void SceneWorld::AddCameraToSceneDatabase(engine::Entity entity)
{
	engine::CameraComponent* pCameraComponent = GetCameraComponent(entity);
	cd::Transform CameraTransform = GetTransformComponent(entity)->GetTransform();
	if (!pCameraComponent)
	{
		assert("Invalid entity");
		return;
	}

	std::string cameraName = "Untitled_Camera";
	if (const engine::NameComponent* pNameComponent = GetNameComponent(entity))
	{
		cameraName = pNameComponent->GetName();
	}

	cd::SceneDatabase* pSceneDatabase = GetSceneDatabase();
	cd::Camera camera(cd::CameraID(pSceneDatabase->GetCameraCount()), cameraName.c_str());
	camera.SetEye(CameraTransform.GetTranslation());
	//camera.SetLookAt(CameraTransform.GetLookAt());
	//camera.SetUp(CameraTransform.GetUp());
	camera.SetNearPlane(pCameraComponent->GetNearPlane());
	camera.SetFarPlane(pCameraComponent->GetFarPlane());
	camera.SetAspect(pCameraComponent->GetAspect());
	camera.SetFov(pCameraComponent->GetFov());
	pSceneDatabase->AddCamera(cd::MoveTemp(camera));
}

void SceneWorld::AddLightToSceneDatabase(engine::Entity entity)
{
	engine::LightComponent* pLightComponent = GetLightComponent(entity);
	if (!pLightComponent)
	{
		assert("Invalid entity");
		return;
	}

	std::string lightName = "Untitled_Light";
	if (const engine::NameComponent* pNameComponent = GetNameComponent(entity))
	{
		lightName = pNameComponent->GetName();
	}

	cd::SceneDatabase* pSceneDatabase = GetSceneDatabase();
	cd::Light light(cd::LightID(pSceneDatabase->GetLightCount()), pLightComponent->GetType());
	light.SetName(lightName.c_str());
	light.SetIntensity(pLightComponent->GetIntensity());
	light.SetRange(pLightComponent->GetRange());
	light.SetRadius(pLightComponent->GetRadius());
	light.SetWidth(pLightComponent->GetWidth());
	light.SetHeight(pLightComponent->GetHeight());
	light.SetAngleScale(pLightComponent->GetAngleScale());
	light.SetAngleOffset(pLightComponent->GetAngleOffset());
	light.SetPosition(pLightComponent->GetPosition());
	light.SetColor(pLightComponent->GetColor());
	light.SetDirection(pLightComponent->GetDirection());
	light.SetUp(pLightComponent->GetUp());
	pSceneDatabase->AddLight(cd::MoveTemp(light));
}

void SceneWorld::AddMaterialToSceneDatabase(engine::Entity entity)
{
	engine::MaterialComponent* pMaterialComponent = GetMaterialComponent(entity);
	assert(pMaterialComponent && "Invalid material entity");
	cd::Material* pMaterialData = pMaterialComponent->GetMaterialData();
	if (!pMaterialData)
	{
		return;
	}

	pMaterialData->SetFloatProperty(cd::MaterialPropertyGroup::Metallic, cd::MaterialProperty::Factor, pMaterialComponent->GetMetallicFactor());
	pMaterialData->SetFloatProperty(cd::MaterialPropertyGroup::Roughness, cd::MaterialProperty::Factor, pMaterialComponent->GetRoughnessFactor());
	pMaterialData->SetBoolProperty(cd::MaterialPropertyGroup::General, cd::MaterialProperty::TwoSided, pMaterialComponent->GetTwoSided());

#if 0
	std::vector<const char*> removeMaterialNames = { "Floor_Tiles_03", "WoodFloor" };
	for (auto& name : removeMaterialNames)
	{
		if (strcmp(pMaterialData->GetName(), name) == 0)
		{
			pMaterialData->RemoveTexture(cd::MaterialPropertyGroup::Metallic);
			pMaterialData->RemoveTexture(cd::MaterialPropertyGroup::Occlusion);
			pMaterialData->RemoveTexture(cd::MaterialPropertyGroup::Roughness);
		}
	}
#endif

	for (int textureTypeValue = 0; textureTypeValue <static_cast<int>(cd::MaterialTextureType::Count); ++textureTypeValue)
	{
		if (MaterialComponent::TextureInfo* textureInfo = pMaterialComponent->GetTextureInfo(static_cast<cd::MaterialPropertyGroup>(textureTypeValue)))
		{
			pMaterialData->SetVec2fProperty(static_cast<cd::MaterialPropertyGroup>(textureTypeValue), cd::MaterialProperty::UVOffset, textureInfo->GetUVOffset());
			pMaterialData->SetVec2fProperty(static_cast<cd::MaterialPropertyGroup>(textureTypeValue), cd::MaterialProperty::UVScale, textureInfo->GetUVScale());
		}
	}
}

void SceneWorld::InitDDGISDK()
{
#ifdef ENABLE_DDGI_SDK
	if (InitDDGI(DDGI_SDK_PATH))
	{
		CD_ENGINE_FATAL("Init DDGI client success at : {0}", DDGI_SDK_PATH);
	}
	else
	{
		CD_ENGINE_FATAL("Init DDGI client failed at : {0}", DDGI_SDK_PATH);
	}
#endif 
}

void SceneWorld::Update()
{
#ifdef ENABLE_DDGI_SDK
	// Send request 30 times per second.
	static auto startTime = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime).count() <= 33 * 1000 * 1000)
	{
		return;
	}
	startTime = std::chrono::steady_clock::now();

	engine::DDGIComponent* pDDGIComponent = GetDDGIComponent(GetDDGIEntity());
	if (!pDDGIComponent)
	{
		CD_ENGINE_FATAL("Can not get DDGI component.");
		return;
	}

	std::shared_ptr<CurrentFrameDecodeData> curDecodeData = GetCurDDGIFrameData();
	if (curDecodeData != nullptr)
	{
		CD_ENGINE_FATAL("Receive DDGI raw data success.");

		// static uint32_t frameCount = 0;
		// static std::string savaPath = (std::filesystem::path(DDGI_SDK_PATH) / "Save").string();
		// WriteDdgi2BinFile(savaPath, *curDecodeData, frameCount++);

		pDDGIComponent->SetDistanceRawData(curDecodeData->visDecodeData);
		pDDGIComponent->SetIrradianceRawData(curDecodeData->irrDecodeData);
	}
#endif
}

}
#include "Common.hpp"
#include "Layout.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "Utils/Util.h"
using namespace std::string_literals;

Layout* g_layout;

void SpriteRenderer::Setup(std::shared_ptr<Shader> shader)
{
	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	
	this->shader = shader;

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite(Texture* tex, glm::vec2 pos0, glm::vec2 pos1, glm::vec2 uv0, glm::vec2 uv1,
	glm::vec4 c0, glm::vec4 c1, glm::vec4 c2, glm::vec4 c3)
{
	float verData[] = {
	//  pos         tex           color
		0.0f, 1.0f, uv0.x, uv1.y, c2.r, c2.g, c2.b, c2.a,
		1.0f, 0.0f, uv1.x, uv0.y, c1.r, c1.g, c1.b, c1.a,
		0.0f, 0.0f, uv0.x, uv0.y, c0.r, c0.g, c0.b, c0.a,

		0.0f, 1.0f, uv0.x, uv1.y, c2.r, c2.g, c2.b, c2.a,
		1.0f, 1.0f, uv1.x, uv1.y, c3.r, c3.g, c3.b, c3.a,
		1.0f, 0.0f, uv1.x, uv0.y, c1.r, c1.g, c1.b, c1.a,
	};

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verData), verData, GL_STREAM_DRAW);

	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
	shader->setMat4("projection", projection);

	glm::vec3 position = glm::vec3(pos0, 0.0f);
	glm::vec3 scale = glm::vec3(pos1 - pos0, 1.0f);
	this->shader->use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::translate(model, glm::vec3(1280 / 2, 720 / 2, 0));
	model = glm::scale(model, scale);
	shader->setMat4("model"s, model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->getOglId());
	shader->setInt("tex"s, 0);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

Layout::Layout()
{

}

Layout::~Layout()
{

}

void Layout::Setup(std::shared_ptr<ShaderLibrary> shaderLibrary)
{
	spriteRenderer.Setup(shaderLibrary->GetShader("sprite"s));
}

bool Layout::LoadL2D(std::string filepath)
{
	std::ifstream ifs = std::ifstream(filepath, std::ios_base::binary);
	if (!ifs.good()) return false;

	try
	{
		this->loadedFile = L2DFile::ReadL2DFile(ifs);
	}
	catch (std::runtime_error ex)
	{
		std::cerr << "[LAYOUT] Failed loading file " << filepath << "!" << std::endl;
		std::cerr << "[LAYOUT] Error details: " << ex.what() << std::endl;
		return false;
	}

	for (SQ2PFile& sq2p : loadedFile.sq2p)
	{
		SP2File& sp2 = sq2p.sp2;
		SQ2File& sq2 = sq2p.sq2;
		//BBS::CTextureObject *cto = new BBS::CTextureObject();
		Texture* tm2 = TextureFromTM2(sq2p.tm2);
		//cto->LoadTM2(sq2p.tm2);
		//cto->CreateTexture();
		//texObjects.push_back(cto);
		//Texture* tm2 = cto->texture;
		textures.push_back(tm2);

		SpriteSet ss;
		ss.texture = tm2;
		for (auto& spr : sp2.sprites)
		{
			Sprite sprite;
			for (int pid = 0; pid < spr.GroupValue; pid++)
			{
				SP2Group& group = sp2.groups[spr.GroupIDX + pid];
				SP2Part& rect = sp2.parts[group.IDXParts];
				SprPart part;
				part.rect.U0 = (float)rect.U0 / (float)tm2->getWidth();
				part.rect.V0 = (float)rect.V0 / (float)tm2->getHeight();
				part.rect.U1 = (float)rect.U1 / (float)tm2->getWidth();
				part.rect.V1 = (float)rect.V1 / (float)tm2->getHeight();
				part.rect.RGBA0 = rect.RGBA0;
				part.rect.RGBA1 = rect.RGBA1;
				part.rect.RGBA2 = rect.RGBA2;
				part.rect.RGBA3 = rect.RGBA3;
				part.X0 = group.X0;
				part.Y0 = group.Y0;
				part.X1 = group.X1;
				part.Y1 = group.Y1;
				sprite.parts.push_back(part);
			}
			ss.sprites.push_back(sprite);
		}
		spriteSets.push_back(ss);

		SequenceSet sqSet;
		sqSet.spriteSetID = spriteSets.size() - 1;
		for (int seqid = 0; seqid < sq2.header.SequenceCount; seqid++)
		{
			Sequence sequence;
			sequence.ID = sq2.sequenceIDs[seqid];
			sequence.name = std::string(sq2.sequenceNames[seqid].name);
			for (int ctrlid = 0; ctrlid < sq2.sequences[seqid].ControlNumber; ctrlid++)
			{
				SequenceChannel channel;
				SQ2Control& ctrl = sq2.controls[sq2.sequences[seqid].ControlIDX + ctrlid];
				channel.maxFrame = ctrl.MaxFrame;
				channel.returnFrame = ctrl.ReturnFrame;
				channel.loopNumber = ctrl.LoopNumber;
				for (int animid = 0; animid < ctrl.AnimationNumber; animid++)
				{
					SQ2Animation& anim = sq2.animations[ctrl.AnimationIDX + animid];
					SequenceAnimation animation{};
					animation.maxFrameNumber = anim.MaxFrameNumber;
					animation.spriteNumber = anim.SpriteNumber;
					animation.keyStartOffset = anim.NOfsKeyData;
					for (int i = 0; i < 11; i++) animation.keyData[i] = anim.KeyDataArray[i];
					animation.kind = anim.Kind;
					animation.blendType = anim.BlendType;
					animation.bDitherOff = anim.bDitherOff;
					animation.bBilinear = anim.bBilinear;
					animation.scissorNumber = anim.ScissorNumber;
					animation.zDepth = anim.ZDepth;

					channel.animations.push_back(animation);
				}
				sequence.controls.push_back(channel);
			}
			sqSet.sequences.push_back(sequence);
		}
		sqSet.keys = sq2.keys;
		sequenceSets.push_back(sqSet);
	}

	// TODO: Build the curves

	return true;
}

void Layout::gui_SequencesList()
{
	if (ImGui::Begin("Sequence List"))
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("L2D File"))
		{
			for (int i = 0; i < sequenceSets.size(); i++)
			{
				SequenceSet& sequenceSet = sequenceSets.at(i);
				if (ImGui::TreeNode((void*)(intptr_t)i, "Sequence Set %d", i))
				{
					for (int j = 0; j < sequenceSet.sequences.size(); j++)
					{
						Sequence& sequence = sequenceSet.sequences.at(j);
						if (ImGui::TreeNode((void*)(intptr_t)j, "Sequence %d", j))
						{
							ImGui::Text("ID: %d", sequence.ID);
							ImGui::SameLine();	ImGui::Text("Name: %s", sequence.name.data());
							if (ImGui::TreeNode("Channels"))
							{
								for (int k = 0; k < sequence.controls.size(); k++)
								{
									SequenceChannel& channel = sequence.controls.at(k);
									if (ImGui::TreeNode((void*)(intptr_t)k, "Channel %d", k))
									{
										ImGui::Text("Max Frame: %d", channel.maxFrame);
										ImGui::Text("Return Frame: %d, Loop Number: %d", channel.returnFrame, channel.loopNumber);
										if (ImGui::TreeNode("Animations"))
										{
											for (int l = 0; l < channel.animations.size(); l++)
											{
												SequenceAnimation& animation = channel.animations.at(l);
												if (ImGui::TreeNode((void*)(intptr_t)l, "Animation %d", l))
												{
													ImGui::Text("Max Frame: %d", animation.maxFrameNumber);
													ImGui::Text("Sprite: %d", animation.spriteNumber);
													ImGui::Text("Key Start: %d", animation.keyStartOffset);
													ImGui::Text("Key Data:");
													ImGui::Text("\t%02X %02X %02X %02X %02X %02X", animation.keyData[0], animation.keyData[1], animation.keyData[2], animation.keyData[3], animation.keyData[4], animation.keyData[5]);
													ImGui::Text("\t%02X %02X %02X %02X %02X", animation.keyData[6], animation.keyData[7], animation.keyData[8], animation.keyData[9], animation.keyData[10]);
													ImGui::Text("Kind: %d, Blend: %d", animation.kind, animation.blendType);
													ImGui::Text("Dither: %s, Bilinear: %s", animation.bDitherOff ? "OFF" : "ON", animation.bBilinear ? "ON" : "OFF");
													ImGui::Text("Scissor: %d, zDepth: %d", animation.scissorNumber, animation.zDepth);

													ImGui::TreePop();
												}
											}
											ImGui::TreePop();
										}
										ImGui::TreePop();
									}
								}
								ImGui::TreePop();
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

	}

	ImGui::End();
}

void Layout::gui_KeysList()
{
	if (ImGui::Begin("Keys List"))
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("L2D File"))
		{
			for (int i = 0; i < sequenceSets.size(); i++)
			{
				SequenceSet& sequenceSet = sequenceSets.at(i);
				if (ImGui::TreeNode((void*)(intptr_t)i, "Sequence Set %d", i))
				{
					for (int j = 0; j < sequenceSet.keys.size(); j++)
					{
						SQ2Key& key = sequenceSet.keys.at(j);
						if (ImGui::TreeNode((void*)(intptr_t)j, "Key %d", j))
						{
							ImGui::Text("Time: %f", key.Key);
							ImGui::Text("Value: %d/%f", key.iValue, key.fValue);
							ImGui::Text("Data: %02X %02X %02X %02X", key.Data[0], key.Data[1], key.Data[2], key.Data[3]);
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

// TODO: This function probably needs re-writing and moving somewhere more general
Texture* Layout::TextureFromTM2(Tm2File& tm2)
{
	Tm2PictureHeader* picture_header = &tm2.pictures.at(0).header;
	uint32 imageSize = (picture_header->imageSize);

	//uint8* imageData = (uint8*)(picture_header + 1);
	uint8* imageData = tm2.pictures.at(0).pixelData.data();
	//uint8* clutData = (imageData + imageSize);
	uint8* clutData = tm2.pictures.at(0).clutData.data();

	uint32 psWidth = (1 << picture_header->gsTex0b.TW);
	uint32 psHeight = (1 << picture_header->gsTex0b.TH);
	uint32 width = picture_header->width;
	uint32 height = picture_header->height;
	uint32 rem = (psWidth - width) << 2;

	uint8* tempImage = (uint8*)malloc(psWidth * psHeight * 4);
	uint8* dstPixel = tempImage;

	//if (psWidth == 0 || psHeight == 0 || width == 0 || height == 0) continue;

	uint32 _x = 0;
	uint32 _y = 0;
	for (_y = 0; _y < height; ++_y)
	{
		for (_x = 0; _x < width; ++_x, dstPixel += 4, ++imageData)
		{
			uint32 pixelIndex0 = *imageData;
			uint32 pixelIndex1 = pixelIndex0;
			bool needClut = false;
			uint16 pixel = 0;

			switch (picture_header->imageType)
			{
			case IT_A1BGR5:
				pixel = *((uint16*)(imageData));
				dstPixel[0] = (pixel & 0x1F) * (1.0 / 31.0 * 255.0);
				dstPixel[1] = ((pixel >> 5) & 0x1F) * (1.0 / 31.0 * 255.0);
				dstPixel[2] = ((pixel >> 10) & 0x1F) * (1.0 / 31.0 * 255.0);
				dstPixel[3] = (pixel & 0x8000) ? (0xFF) : (0);
				imageData += 1;
				break;
			case IT_XBGR8:
				dstPixel[0] = imageData[0];
				dstPixel[1] = imageData[1];
				dstPixel[2] = imageData[2];
				imageData += 2;
				break;
			case /*IT_ABGR8*/IT_RGBA:
				dstPixel[0] = imageData[0];
				dstPixel[1] = imageData[1];
				dstPixel[2] = imageData[2];
				dstPixel[3] = imageData[3];
				imageData += 3;
				break;
			case IT_CLUT4:
				needClut = true;
				pixelIndex0 &= 0x0F;
				pixelIndex1 = (pixelIndex1 >> 4) & 0x0F;
				break;
			case IT_CLUT8:
			{
				needClut = true;
				if ((pixelIndex0 & 31) >= 8)
				{
					if ((pixelIndex0 & 31) < 16)
					{
						pixelIndex0 += 8;				// +8 - 15 to +16 - 23
					}
					else if ((pixelIndex0 & 31) < 24)
					{
						pixelIndex0 -= 8;				// +16 - 23 to +8 - 15
					}
				}
			}
			break;
			default:
				std::cerr << "UNKNOWN IMAGE TYPE " << picture_header->imageType << std::endl;
				//return;
				break;
			}

			if (!needClut) continue;

			switch (picture_header->clutType)
			{
			case CT_NONE:
				break;
			case CT_A1BGR5:
				pixelIndex0 <<= 1;
				pixel = *((uint16*)(clutData + pixelIndex0));
				dstPixel[0] = (pixel & 0x1F) * (1.0 / 31.0 * 255.0);
				dstPixel[1] = ((pixel >> 5) & 0x1F) * (1.0 / 31.0 * 255.0);
				dstPixel[2] = ((pixel >> 10) & 0x1F) * (1.0 / 31.0 * 255.0);
				dstPixel[3] = (pixel & 0x8000) ? (0xFF) : (0);
				break;
			case CT_XBGR8:
				pixelIndex0 *= 3;
				dstPixel[0] = clutData[pixelIndex0 + 0];
				dstPixel[1] = clutData[pixelIndex0 + 1];
				dstPixel[2] = clutData[pixelIndex0 + 2];
				dstPixel[3] = 0xFF;
				break;
			case CT_ABGR8:
				pixelIndex0 <<= 2;
				dstPixel[0] = clutData[pixelIndex0 + 0];
				dstPixel[1] = clutData[pixelIndex0 + 1];
				dstPixel[2] = clutData[pixelIndex0 + 2];
				dstPixel[3] = clutData[pixelIndex0 + 3];
				break;
			default:
				std::cerr << "UNKNOWN CLUT TYPE " << picture_header->clutType << std::endl;
				//return;
				break;
			}

			if (picture_header->imageType == IT_CLUT4)
			{
				dstPixel += 4;
				++_x;
				switch (picture_header->clutType)
				{
				case CT_A1BGR5:
					pixelIndex1 <<= 1;
					pixel = *((uint16*)(clutData + pixelIndex1));
					dstPixel[0] = (pixel & 0x1F) * (1.0 / 31.0 * 255.0);
					dstPixel[1] = ((pixel >> 5) & 0x1F) * (1.0 / 31.0 * 255.0);
					dstPixel[2] = ((pixel >> 10) & 0x1F) * (1.0 / 31.0 * 255.0);
					dstPixel[3] = (pixel & 0x8000) ? (0xFF) : (0);
					break;
				case CT_XBGR8:
					pixelIndex1 *= 3;
					dstPixel[0] = clutData[pixelIndex1 + 0];
					dstPixel[1] = clutData[pixelIndex1 + 1];
					dstPixel[2] = clutData[pixelIndex1 + 2];
					dstPixel[3] = 0xFF;
					break;
				case CT_ABGR8:
					pixelIndex1 <<= 2;
					dstPixel[0] = clutData[pixelIndex1 + 0];
					dstPixel[1] = clutData[pixelIndex1 + 1];
					dstPixel[2] = clutData[pixelIndex1 + 2];
					dstPixel[3] = clutData[pixelIndex1 + 3];
					break;
				default:
					std::cerr << "UNKNOWN CLUT TYPE " << picture_header->clutType << std::endl;
					//return;
					break;
				}
			}
		}
		dstPixel += rem;
	}

	Texture* actualTexture = new Texture(psWidth, psHeight, tempImage, PF_RGBA32, /*tex_entry.second*/nullptr);
	return actualTexture;

	/*Tm2Picture& picture = tm2.pictures.at(0);
	//Texture* tex = new Texture(width, height, data, format, usrPtr);
	uint32 dataWidth = picture.header.width;
	uint32 dataHeight = picture.header.height;

	uint32 bufferWidth = (1 << picture.header.gsTex0b.TW);
	uint32 bufferHeight = (1 << picture.header.gsTex0b.TH);

	std::vector<uint8> decodedData = std::vector<uint8>();
	decodedData.reserve(4 * bufferWidth * bufferHeight);

	uint8* pixelData = picture.pixelData.data();
	int pixelPtr = 0;
	uint8* clutData = picture.clutData.data();
	uint32 rem = (bufferWidth - dataWidth) * 4;

	for (int py = 0; py < dataHeight; py++)
	{
		for (int px = 0; px < dataWidth; px++)
		{
			// STEP 1: Decode clut index
			uint32 pixelIndex0 = pixelData[pixelPtr];
			uint32 pixelIndex1 = pixelIndex0;

			switch (picture.header.imageType)
			{
			case IT_RGBA:	// No CLUT.
				break;
			case IT_CLUT4:	// 4 bit CLUT => 2 pixels per byte
				pixelIndex0 &= 0x0F;
				pixelIndex1 = (pixelIndex1 >> 4) & 0x0F;
				break;
			case IT_CLUT8:	// 8 bit CLUT
				if ((pixelIndex0 & 31) >= 8)
				{
					if ((pixelIndex0 & 31) < 16)
					{
						pixelIndex0 += 8;				// +8 - 15 to +16 - 23
					}
					else if ((pixelIndex0 & 31) < 24)
					{
						pixelIndex0 -= 8;				// +16 - 23 to +8 - 15
					}
				}
				break;
			default:
				// TODO: Error
				break;
			}

			// STEP 2: Decode first pixel from data/clut
			uint16 temp16;
			switch (picture.header.clutType)
			{
			case CT_NONE:
				decodedData.push_back(pixelData[pixelPtr++]);
				decodedData.push_back(pixelData[pixelPtr++]);
				decodedData.push_back(pixelData[pixelPtr++]);
				decodedData.push_back(pixelData[pixelPtr++]);
				break;
			case CT_A1BGR5:
				pixelIndex0 *= 2;
				temp16 = clutData[pixelIndex0 + 0] | (clutData[pixelIndex0 + 1] << 8);
				decodedData.push_back((temp16 & 0x1F) * (1.0 / 31.0 * 255.0));
				decodedData.push_back(((temp16 >> 5) & 0x1F) * (1.0 / 31.0 * 255.0));
				decodedData.push_back(((temp16 >> 10) & 0x1F) * (1.0 / 31.0 * 255.0));
				decodedData.push_back((temp16 & 0x8000) ? (0xFF) : (0));
				break;
			case CT_XBGR8:
				pixelIndex0 *= 3;
				decodedData.push_back(clutData[pixelIndex0 + 0]);
				decodedData.push_back(clutData[pixelIndex0 + 1]);
				decodedData.push_back(clutData[pixelIndex0 + 2]);
				decodedData.push_back(0xFF);
				break;
			case CT_ABGR8:
				pixelIndex0 *= 4;
				decodedData.push_back(clutData[pixelIndex0 + 0]);
				decodedData.push_back(clutData[pixelIndex0 + 1]);
				decodedData.push_back(clutData[pixelIndex0 + 2]);
				decodedData.push_back(clutData[pixelIndex0 + 3]);
				break;
			default:
				// TODO: Error
				break;
			}

			// STEP 3: handle possible 2nd pixel
			if (picture.header.imageType == IT_CLUT4)
			{
				px++;
				switch (picture.header.clutType)
				{
					// NOTE: No CT_NONE, as imageType == IT_CLUT4 && clutType == CT_NONE would be invalid
				case CT_A1BGR5:
					pixelIndex0 *= 2;
					temp16 = clutData[pixelIndex0 + 0] | (clutData[pixelIndex0 + 1] << 8);
					decodedData.push_back((temp16 & 0x1F) * (1.0 / 31.0 * 255.0));
					decodedData.push_back(((temp16 >> 5) & 0x1F) * (1.0 / 31.0 * 255.0));
					decodedData.push_back(((temp16 >> 10) & 0x1F) * (1.0 / 31.0 * 255.0));
					decodedData.push_back((temp16 & 0x8000) ? (0xFF) : (0));
					break;
				case CT_XBGR8:
					pixelIndex0 *= 3;
					decodedData.push_back(clutData[pixelIndex0 + 0]);
					decodedData.push_back(clutData[pixelIndex0 + 1]);
					decodedData.push_back(clutData[pixelIndex0 + 2]);
					decodedData.push_back(0xFF);
					break;
				case CT_ABGR8:
					pixelIndex0 *= 4;
					decodedData.push_back(clutData[pixelIndex0 + 0]);
					decodedData.push_back(clutData[pixelIndex0 + 1]);
					decodedData.push_back(clutData[pixelIndex0 + 2]);
					decodedData.push_back(clutData[pixelIndex0 + 3]);
					break;
				default:
					// TODO: Error
					break;
				}
			}
		}

		for (int p = 0; p < rem; p++) decodedData.push_back(0);
	}

	return new Texture(bufferWidth, bufferHeight, decodedData.data(), PF_RGBA32, nullptr);*/
}

int Layout::GetSpriteSetCount() { return spriteSets.size(); }

int Layout::GetSpriteCount(int setIdx)
{
	if (setIdx < 0 || setIdx >= GetSpriteSetCount())
	{
		return -1;
	}

	return spriteSets[setIdx].sprites.size();
}

void Layout::DrawSingleSprite(int setIdx, int spriteIdx)
{
	if (setIdx < 0 || setIdx >= GetSpriteSetCount())
	{
		return;
	}

	SpriteSet& set = spriteSets.at(setIdx);
	set.texture->ogl_loadIfNeeded();

	if (spriteIdx < 0 || spriteIdx >= set.sprites.size())
	{
		return;
	}

	Sprite& sprite = set.sprites.at(spriteIdx);
	for (SprPart& part : sprite.parts)
	{
		glm::vec2 pos0 = glm::vec2(part.X0, part.Y0);
		glm::vec2 pos1 = glm::vec2(part.X1, part.Y1);
		glm::vec2 uv0 = glm::vec2(part.rect.U0, part.rect.V0);
		glm::vec2 uv1 = glm::vec2(part.rect.U1, part.rect.V1);
		glm::vec4 c0 = Uint32ToColor(part.rect.RGBA0);
		glm::vec4 c1 = Uint32ToColor(part.rect.RGBA1);
		glm::vec4 c2 = Uint32ToColor(part.rect.RGBA2);
		glm::vec4 c3 = Uint32ToColor(part.rect.RGBA3);
		spriteRenderer.DrawSprite(set.texture,
			pos0, pos1, uv0, uv1,
			c0, c1, c2, c3);
	}

}

void Layout::DrawSequenceFrame(int setID, int sequenceID, int channelID, int frame)
{
	if (setID < 0 || setID >= GetSpriteSetCount())
	{
		return;
	}

	SpriteSet& spriteSet = spriteSets.at(setID);
	SequenceSet& sequenceSet = sequenceSets.at(setID);
	std::vector<SQ2Key>& keyArr = sequenceSet.keys;

	if (sequenceID < 0 || sequenceID >= sequenceSet.sequences.size())
	{
		return;
	}

	Sequence& sequence = sequenceSet.sequences.at(sequenceID);

	if (channelID < 0 || channelID >= sequence.controls.size())
	{
		return;
	}

	SequenceChannel channel = sequence.controls.at(channelID);

	if (frame < 0) frame = 0;
	if (frame > channel.maxFrame) frame = channel.maxFrame;

	spriteSet.texture->ogl_loadIfNeeded();

	/*for each (SequenceAnimation& anim in channel.animations)
	{
		if (anim.spriteNumber < 0) continue;
		Sprite& sprite = spriteSet.sprites.at(anim.spriteNumber);
		//int keyBase = anim.keyStartOffset;

		float curveTime = (float)frame;

		float baseX = anim.animCurves.baseXCurve.evaluate(curveTime);
		float baseY = anim.animCurves.baseYCurve.evaluate(curveTime);
		float offsetX = anim.animCurves.offsetXCurve.evaluate(curveTime);
		float offsetY = anim.animCurves.offsetYCurve.evaluate(curveTime);
		// TODO Rotate
		// TODO Scale
		// TODO Color

		glm::vec2 pivot = glm::vec2(baseX, baseY);
		glm::vec2 offset = glm::vec2(offsetX, offsetY);
		//offset = glm::rotate(offset, rotateZ?);
		glm::vec2 scale = glm::vec2(1.f, 1.f);
	}*/
}
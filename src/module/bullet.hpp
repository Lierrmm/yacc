#pragma once

class bullet final : public module
{
public:
	void post_load() override;

private:
	static const game::native::dvar_t* bg_bulletRange;

	static DWORD bullet_fire_addr;
	static void bullet_fire_stub();
};

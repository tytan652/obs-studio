#pragma once

#include "auth-oauth.hpp"

class RestreamAuth : public OAuthStreamKey {
	Q_OBJECT

	bool uiLoaded = false;

	virtual bool RetryLogin() override;

	virtual bool LoadInternal() override;

	bool GetChannelInfo();

	virtual void LoadUI() override;

public:
	RestreamAuth(const Def &d);
	~RestreamAuth();

	static std::shared_ptr<Auth> Login(QWidget *parent,
					   const std::string &service_name);
};

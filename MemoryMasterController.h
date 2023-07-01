#pragma once
#include "MemoryMaster.h"
#include "MemoryMasterModel.h"

class MemoryMasterController : public QObject
{
	Q_OBJECT
public:
	MemoryMasterController(MemoryMaster&, MemoryMasterModel&);
	~MemoryMasterController();
	void connectSignalAndSlot();

private:
	MemoryMaster* m_view;
	MemoryMasterModel* m_model;
};
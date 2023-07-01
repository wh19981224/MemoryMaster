#include "MemoryMasterController.h"

MemoryMasterController::MemoryMasterController(MemoryMaster &mainwindow, MemoryMasterModel &qobj) :
	m_view(&mainwindow),
	m_model(&qobj)
{
	connectSignalAndSlot();
}

MemoryMasterController::~MemoryMasterController()
{
	
}

void MemoryMasterController::connectSignalAndSlot()
{
	// view����model�㷢���ź�
	connect(m_view, SIGNAL(pathChanged(QString)), m_model, SLOT(receiveOnePath(QString)));
	connect(m_view, SIGNAL(startCompute()), m_model, SLOT(startCompute()));

	// model����view�㷢���ź�
	connect(m_model, SIGNAL(finishCompute(QStringList, QStringList, QStringList)),
		    m_view, SLOT(updateListWidget(QStringList, QStringList, QStringList)));
	connect(m_model, SIGNAL(unvalidPath()), m_view, SLOT(setStateHitLabelUnvalidPath()));
}
#include "../dataTransfer/dataTransfer/dataTransfer.h"
#include "json.hpp"

class CFileCopier : public datatransfer::filetransfer::CTransferControl
{
#pragma region Public_Method
public:
//==============================================================================
	/**
	* ����������� ��������� �������� ������ ��� �����.
	* @param sAccessFlag - ����� �������.
	* @param directoryPath - ���������� ���� �� ����� ���������.
	* @param pIocp - �������� Iocp.
	*/
	CFileCopier(
		const SAccessFlag& sAccessFlag,
		const std::filesystem::path directoryPath,
		const std::shared_ptr<wname::io::iocp::CIocp>& pIocp) :
		CTransferControl(sAccessFlag, directoryPath, pIocp)
	{

	}
//==============================================================================
	/**
	* ����������� ��������� �������� ������ ��� TCP �������.
	* @param sAccessFlag - ����� �������.
	* @param strIp - IP ����� �������.
	* @param wPort - ���� �������.
	* @param pIocp - �������� Iocp.
	*/
	CFileCopier(
		const SAccessFlag& sAccessFlag,
		const std::string strIp,
		const WORD wPort,
		const std::shared_ptr<wname::io::iocp::CIocp>& pIocp) :
		CTransferControl(sAccessFlag, strIp, wPort, pIocp)
	{

	}
//==============================================================================
	/**
	* ��������� ������.
	* @param bIsWait - ������� ��������.
	*/
	void release(
		const bool bIsWait) noexcept override
	{
		__super::release(bIsWait);
	}
//==============================================================================
	/**
	* ����������.
	*/
	~CFileCopier()
	{
		release(true);
	}
//==============================================================================
	CFileCopier(const CFileCopier&) = delete;
	CFileCopier(CFileCopier&&) = delete;
	CFileCopier& operator=(const CFileCopier&) = delete;
	CFileCopier& operator=(CFileCopier&&) = delete;
//==============================================================================
#pragma endregion

#pragma region Protected_Method
protected:
//==============================================================================
	/**
	* �������� �������.
	* @param eProgressRequest - ��� ���������.
	* @param transferRequest - ������ �� ��������.
	*/
	void progressRequestHandler(
		const EProgressRequest eProgressRequest,
		const CTransferRequest& transferRequest) noexcept override
	{
		switch (eProgressRequest)
		{
		case EProgressRequest::eCreate:
		{
			wprintf(L"\n����� ������������� ������ � \"%s\"",
				transferRequest._requestPath.c_str());
			break;
		}
		case EProgressRequest::eStartRequest:
		{
			wprintf(L"\n������ ��������� ������ ��� \"%s\"",
				transferRequest._requestPath.c_str());
			break;
		}
		case EProgressRequest::eEndRequest:
		{
			wprintf(L"\n��������� ��������� ������ ��� \"%s\"",
				transferRequest._requestPath.c_str());
			break;
		}
		case EProgressRequest::eStartTransfer:
		{
			wprintf(L"\n������ �������� %llu ������ ��� \"%s\"",
				transferRequest._files.size(),
				transferRequest._requestPath.c_str());
			break;
		}
		case EProgressRequest::eEndTransfer:
		{
			wprintf(L"\n��������� �������� ������ ��� \"%s\"",
				transferRequest._requestPath.c_str());
			break;
		}
		case EProgressRequest::eDelete:
		{
			wprintf(L"\n��������� �������������� ������� � \"%s\"",
				transferRequest._requestPath.c_str());
			break;
		}
		default:
			break;
		}
	}
//==============================================================================
	/**
	* �������� �������� ����� ��� �������.
	* @param eProgressTransferFileHandler - ��� ���������.
	* @param transferRequest - ������ �� ��������.
	* @param transferFile - ������������ ����.
	*/
	void progressTransferFileHandler(
		const EProgressTransferFileHandler eProgressTransferFileHandler,
		const CTransferRequest& transferRequest,
		const CTransferFile& transferFile) noexcept override
	{
		UNREFERENCED_PARAMETER(transferRequest);

		switch (eProgressTransferFileHandler)
		{
		case EProgressTransferFileHandler::eCreate:
		{
			wprintf(L"\n������ �������� ����� \"%s\" ������� %llu",
				&transferFile._fileInfo.wszFilePath[0],
				transferFile._fileInfo.uSize);
			break;
		}
		case EProgressTransferFileHandler::eStartTransfer:
		{
			wprintf(L"\n������ �������� ����� \"%s\"",
				&transferFile._fileInfo.wszFilePath[0]);
			break;
		}
		case EProgressTransferFileHandler::eProgress:
		{
			#pragma warning(disable: 26493)
			if(transferFile._uWriteSize == 0 || transferFile._uReadSize == 0)
				break;

			const auto currentCount = GetTickCount64();
			const auto difCount = currentCount - _count;
			_count = currentCount;
			auto writePercent = (double)transferFile._uWriteSize / (double)transferFile._fileInfo.uSize;
			writePercent*= 100;
			auto readPercent = (double)transferFile._uReadSize / (double)transferFile._fileInfo.uSize;
			readPercent *= 100;

			if (transferFile._uWriteSize != transferFile._fileInfo.uSize ||
				transferFile._uReadSize != transferFile._fileInfo.uSize)
			{
				if(difCount < 10)
					break;
			}
			
			wprintf(L"\n�������� �������� ����� \"%s\" �������: %.2f%% �������� %.2f%%",
				&transferFile._fileInfo.wszFilePath[0],
				readPercent,
				writePercent);		
					
			break;
		}
		case EProgressTransferFileHandler::eEndTransfer:
		{
			wprintf(L"\n��������� �������� ����� \"%s\"",
				&transferFile._fileInfo.wszFilePath[0]);
			break;
		}
		case EProgressTransferFileHandler::eDelete:
		{
			wprintf(L"\n��������� ������� �������� ����� \"%s\"",
				&transferFile._fileInfo.wszFilePath[0]);
			break;
		}
		default:
			break;
		}
	}
//==============================================================================
#pragma endregion

#pragma region Private_Data
private:
//==============================================================================
	/** ��� ���������� ������ ��������� ��� ����� */
	UINT64 _count = 0;
//==============================================================================
#pragma endregion
};


int wmain(int argc, PWCHAR* argv)
{
	setlocale(LC_ALL, "Russian");
	try
	{
		if (argc < 2)
		{
			nlohmann::json j =
				R"(
	{
		"source": 
		{
			"IsCopy": true,
			"IsOverrit": true,
			"IsPaste": true,
			"directory": 
			{
				"path": "g:\\FirstStart\\"
			}
		},
		"recipients": 
		[
			{
				"IsCopy": true,
				"IsOverrit": true,
				"IsPaste": true,
				"network": 
				{
					"ip": "127.0.0.1",
					"port": 1337
				}
			},
			{
				"IsCopy": true,
				"IsOverrit": true,
				"IsPaste": true,
				"directory": 
				{
					"path": "C:\\test\\recipient"
				}
			}
		],
		"requests": 
		[
			{
				"type": "copy",
				"path": "C:\\test\\recipient"
			}
		]
	}
	)"_json;
			wprintf(L"\n������������: fileCopier path");
			wprintf(L"\n���������:");
			wprintf(L"\n\tpath\t���� �� ����� �������� json");
			wprintf(L"\n������:\n%S", j.dump(4).c_str());
			return 0;
		}


		nlohmann::json j;
		std::ifstream(argv[1]) >> j;
		auto pIocp = std::make_shared<wname::io::iocp::CIocp>();

		auto source = j["source"];
		CFileCopier::SAccessFlag flag;
		flag.bIsCopy = source["IsCopy"].get<bool>();
		flag.bIsOverrite = source["IsOverrit"].get<bool>();
		flag.bIsPaste = source["IsPaste"].get<bool>();
		std::unique_ptr<CFileCopier> pControl;
		if (auto directory = source.find("directory"); directory != source.end())
		{
			const auto& nodeDirectory = directory.value();
			pControl = make_unique<CFileCopier>(
				flag,
				nodeDirectory["path"].get<std::string>(),
				pIocp);
		}
		else if (auto network = source.find("network"); network != source.end())
		{
			const auto& nodeNetwork = network.value();
			pControl = make_unique<CFileCopier>(
				flag,
				nodeNetwork["ip"].get<std::string>(),
				nodeNetwork["port"].get<WORD>(),
				pIocp);
		}
		else
		{
			return 0;
		}

		if (auto recipients = j.find("recipients"); recipients != j.end())
		{
			const auto& nodeRecipients = recipients.value();
			for (const auto& recipient : nodeRecipients)
			{
				flag.bIsCopy = recipient["IsCopy"].get<bool>();
				flag.bIsOverrite = recipient["IsOverrit"].get<bool>();
				flag.bIsPaste = recipient["IsPaste"].get<bool>();

				if (auto directory = recipient.find("directory"); directory != recipient.end())
				{
					const auto& nodeDirectory = directory.value();
					pControl->addRecipient(
						flag,
						nodeDirectory["path"].get<std::string>());
				}
				else if (auto network = recipient.find("network"); network != recipient.end())
				{
					const auto& nodeNetwork = network.value();
					pControl->addRecipient(
						flag,
						nodeNetwork["ip"].get<std::string>(),
						nodeNetwork["port"].get<WORD>());
				}
				else
				{
					return 0;
				}
			}
		}

		if (auto requests = j.find("requests"); requests != j.end())
		{
			const auto& nodeRequests = requests.value();
			for (const auto& request : nodeRequests)
			{
				std::error_code ec;
				auto type = request["type"].get<std::string>();
				if (type == "copy")
				{
					ec = pControl->request(
						CFileCopier::ERequest::eCopy,
						request["path"].get<std::string>());
				}
				else if (type == "paste")
				{
					ec = pControl->request(
						CFileCopier::ERequest::ePaste,
						request["path"].get<std::string>());
				}

				if (ec)
				{
					wprintf(L"\n������ \"%S\" �������� � ������� %d\n",
						request["path"].get<std::string>().c_str(),
						ec.value());
				}
				else
				{
					wprintf(L"\n������ \"%S\" ��������\n",
						request["path"].get<std::string>().c_str());
				}

			}
		}
	}
	catch (const std::exception& ex)
	{
		wprintf(L"\n%S\n", ex.what());
	}
	/*std::list<std::unique_ptr<datatransfer::filetransfer::CTransferControl>> list;
	list.push_back(std::make_unique<datatransfer::filetransfer::CTransferControl>(
		flag, L"g:\\FirstStart\\", pIocp));
	list.back()->addRecipient(flag, "127.0.0.1", iMin);


	for (; iMin < iMax; iMin++)
	{
		list.push_back(std::make_unique<datatransfer::filetransfer::CTransferControl>(
			flag, "127.0.0.1", iMin, pIocp));
		list.back()->addRecipient(flag, "127.0.0.1", iMin +1);
	}
	datatransfer::filetransfer::CTransferControl s(flag, "127.0.0.1", iMin, pIocp);
	s.addRecipient(flag, L"C:\\test\\recipient");
	const auto ec = s.request(datatransfer::filetransfer::CTransferControl::ERequest::eCopy,
		L"C:\\test\\recipient");*/

	system("pause");
	return 0;
}
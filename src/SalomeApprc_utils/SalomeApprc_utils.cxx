#include "SalomeApprc_utils.h"

void AddComponents_from_salomeappdir(const QDir& salomeappdir, SUIT_ResourceMgr* ResMgr)
{
	QFileInfoList salomexd_list = salomeappdir.entryInfoList(QStringList() << "*.salomexd",QDir::Files);
	foreach(QFileInfo filename, salomexd_list)
    {
		QFile file( filename.filePath() );
        if ( !file.open( QFile::ReadOnly ) )
        {
            qWarning() << "file " << filename.fileName() << " is not accessible";
            continue;
        }

        QJsonDocument document = QJsonDocument::fromJson( file.readAll() );
        if ( document.isNull() )
        {
            qWarning() << "invalid json file. Filename: " << filename.fileName();
            continue;
        }
        QJsonObject salomexd_dict = document.object();
        QJsonValue components = salomexd_dict.value( "components");

		QString root(salomeappdir.path() + "/__SALOME_EXT__");
		if ( components.isArray() )
		{
			// In the case that we have a list of components. We consider that all of them are GUI module
			foreach ( auto comp, components.toArray())
			{
				AddGuiComponent(comp.toString(), root, ResMgr);
			}
		}
		else
		{
			// In the case that we have a dict of several component group
			QJsonObject compObj = components.toObject();
			foreach ( QString key, compObj.keys() )
		    {
				if ( QString::compare(key, ITERACTIVE_EXTCOMPONENT_KEY) == 0 )
				{
					foreach (auto comp, compObj.value(key).toArray() )
						AddGuiComponent(comp.toString(), root, ResMgr);
				}
			}
		}
    }
}

void AddComponents_from_salomemodules(const QString& salomemodules, const QDir& salomeappdir, SUIT_ResourceMgr* ResMgr)
{
	QRegularExpression sep(":|,");
	QStringList components_list = salomemodules.split(sep,QString::SkipEmptyParts);
	foreach (QString comp, components_list)
	{
		QString comp_root_dir = Qtx::getenv(comp + "_ROOT_DIR");

		qWarning() << comp;
		qWarning() << comp_root_dir;
		if (comp_root_dir.isEmpty())
			comp_root_dir = salomeappdir.path() + "/__SALOME_EXT__";
		AddGuiComponent(comp, comp_root_dir, ResMgr);
	}


}
void AddGuiComponent(const QString& comp, const QString& CompRoot, SUIT_ResourceMgr* ResMgr)
{
	QStringList CompsResMgr = ResMgr->stringValue("launch", "user_modules").split(";", QString::SkipEmptyParts);
	ResMgr->setValue( "user_modules", comp, CompRoot );

	CompsResMgr << comp;
	CompsResMgr.removeDuplicates();
    ResMgr->setValue( "launch", "user_modules", CompsResMgr.join( ";" ) );
}
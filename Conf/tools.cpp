#include "incs/conf_reader.hpp"
#include "incs/conf_parser.hpp"

void parseServer(int argc, char **argv, Conf_parser &_parser)
{
    try
    {
        if (argc == 1 || argc == 2)
        {
            std::string _path;
            if (argc == 1)
                _path = "Conf/config/default.conf";
            else
                _path = argv[1];

            Conf_reader _config(_path);

            if (!_config.validate_file())
                throw Conf_reader::InvalidFileException();
            std::string _file;

            _file = _config.read_conf();

            if (_file.empty())
                throw Conf_reader::InvalidFileException();
            _parser.store_server(_file);
            // now we have our servers block stored in block_conf storage //
            return;
        }
        std::cerr << "Invalid number of arguments" << std::endl;
        return;
    }
    catch (Conf_reader::InvalidFileException &e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (Conf_parser::OutOfServerBlocks &e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (Conf_parser::ErrorEncountered &e)
    {
        std::cerr << e.what() << std::endl;
    }
    // add the catch blocks left : my part + cgi on it //
}
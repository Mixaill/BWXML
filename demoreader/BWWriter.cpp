#include "BWWriter.h"
#include "Base64.h"
#include "BWCommon.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <boost/assign.hpp>

using boost::property_tree::ptree;
using namespace BigWorld;

BWXMLWriter::BWXMLWriter(const std::string& fname)
{
	try
	{
		boost::property_tree::xml_parser::read_xml(fname, mTree, 
			boost::property_tree::xml_parser::trim_whitespace);
	}
	catch(...)
	{
		throw std::exception("XML parsing error");
	};
	if (mTree.size() != 1)
		throw std::exception("XML file must contain only 1 root level node");
	
	mTree.swap(mTree.begin()->second); // swapping the whole tree to its first node
}

void BWXMLWriter::treeWalker(const ptree& node)
{
	for (auto subNode=node.begin(); subNode != node.end(); ++subNode)
	{
		mStrings.push_back(subNode->first);
		treeWalker(subNode->second);
	}
}

void BWXMLWriter::collectStrings()
{
	mStrings.clear();
	treeWalker(mTree);
	std::sort(mStrings.begin(), mStrings.end());
	mStrings.erase(std::unique(mStrings.begin(), mStrings.end()),
		mStrings.end());

	static std::vector<std::string> namesToClear = boost::assign::list_of\
		("<xmlcomment>")("row0")("row1")("row2")("row3");
	for (auto str=namesToClear.begin(); str!=namesToClear.end(); ++str)
	{
		auto strIdx = std::find(mStrings.begin(), mStrings.end(), *str);
		if (strIdx != mStrings.end())
			mStrings.erase(strIdx);
	}
}

int BWXMLWriter::resolveString(const std::string& str)
{
	auto pos = std::find(mStrings.begin(), mStrings.end(), str);
	if (pos == mStrings.end())
		throw std::exception("String key not found!");
	return (pos - mStrings.begin());
}

rawDataBlock BWXMLWriter::serializeNode(const boost::property_tree::ptree& node_value, bool simple)
{
	if (node_value.size() == 4) // maybe that's a matrix?..
	{
		std::vector<boost::optional< const ptree& > > rows;
		for (int i=0; i<4; ++i)
		{
			auto row = node_value.get_child_optional("row"+boost::lexical_cast<std::string>(i));
			if (!row) // bad luck.
				break;
			rows.push_back(row);
		}
		if (rows.size() == 4) // we've found all 4 required rows
		{
			std::stringstream buffer;
			for (auto it=rows.begin(); it!=rows.end(); ++it)
			{
				rawDataBlock block = PackBuffer((*it)->data());
				assert(block.type == BW_Float);	// better safe than sorry
				buffer << block.data;
			}
			return rawDataBlock(BW_Float, buffer.str());
		}
	}

	if (!simple && node_value.size() && (!node_value.get("<xmlcomment>", "N/A").compare("N/A"))) // has sub-nodes
	{
		return rawDataBlock(BW_Section, serializeSection(node_value));
	}

	if (!node_value.get("<xmlcomment>", "").compare("BW_String"))
		return rawDataBlock(BW_String, node_value.data()); 
	return PackBuffer(node_value.data());
}

BigWorld::DataDescriptor BWXMLWriter::BuildDescriptor(rawDataBlock block, int prevOffset)
{
	return DataDescriptor(block.type, prevOffset + block.data.length());
}

void BWXMLWriter::saveTo(const std::string& destname)
{
	collectStrings();
	
	std::stringstream outbuf;
	StreamBufWriter outstream(outbuf.rdbuf());
	outstream.put(BigWorld::PACKED_SECTION_MAGIC);
	outstream.put<char>(0);
	for (auto it = mStrings.begin(); it!= mStrings.end(); ++it)
		outstream.putString(*it);
	outstream.put<char>(0);

	outstream.putString(serializeSection(mTree), false);

	std::ofstream mFile;
	mFile.open(destname, std::ios::binary);
	if (!mFile.is_open())
		throw std::exception("Can't open the file");
	mFile << outbuf.rdbuf();
	mFile.close();

  std::cout << "Success" << std::endl;
}

std::string BWXMLWriter::serializeSection(const ptree& node)
{
	std::stringstream _ret;
	StreamBufWriter ret(_ret.rdbuf());

	rawDataBlock ownData = serializeNode(node, true); // getting own plain content
	dataArray childData;
	for (auto it=node.begin(); it!=node.end(); ++it)
	{
		//std::cout << "For " << it->first << " : ";
		if (!it->first.compare("<xmlcomment>")) // skipping comments
			continue;
		childData.push_back(dataBlock(resolveString(it->first), serializeNode(it->second, false)));
	}

	DataDescriptor ownDescriptor = BuildDescriptor(ownData, 0);
	ret.put<short>(childData.size());
	ret.put<DataDescriptor>(ownDescriptor);
	ret.putString(ownData.data, false);
	
	int currentOffset = ownDescriptor.offset();
	for (auto it=childData.begin(); it!=childData.end(); ++it)
	{
		//std::cout << "off=" << currentOffset << std::endl;
		DataNode bwNode;
		bwNode.nameIdx = it->stringId;
		bwNode.data = BuildDescriptor(it->data, currentOffset);
		ret.put<DataNode>(bwNode);
		currentOffset = bwNode.data.offset();
	}
	
	for (auto it=childData.begin(); it!=childData.end(); ++it)
	{
		ret.putString(it->data.data, false);
	}

	return _ret.str();
}


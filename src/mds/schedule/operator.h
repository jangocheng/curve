/*
 *  Copyright (c) 2020 NetEase Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * Project: curve
 * Created Date: Sun Nov 17 2018
 * Author: lixiaocui
 */


#ifndef SRC_MDS_SCHEDULE_OPERATOR_H_
#define SRC_MDS_SCHEDULE_OPERATOR_H_

#include <vector>
#include <string>
#include <memory>
#include "src/mds/schedule/topoAdapter.h"
#include "src/mds/schedule/operatorStep.h"
#include "src/mds/topology/topology_item.h"
#include "src/mds/common/mds_define.h"

using ::curve::mds::topology::EpochType;
using ::curve::mds::topology::CopySetKey;
using ::std::chrono::steady_clock;

namespace curve {
namespace mds {
namespace schedule {

enum OperatorPriority {
  LowPriority,
  NormalPriority,
  HighPriority
};

class Operator {
 public:
  Operator() = default;
  Operator(EpochType startEpoch, const CopySetKey &id, OperatorPriority pri,
           const steady_clock::time_point &createTime,
           std::shared_ptr<OperatorStep> step);
  /**
   * @brief execute operator
   *
   * @param originInfo 入参, chunkServer上报的copySet的信息
   * @param newInfo 出参, schedule下发给copySet的命令
   *
   * @return 返回值, 此次operator执行是否完成/是否失败/是否有指令下发/错误信息
   */
  ApplyStatus Apply(const CopySetInfo &originInfo, CopySetConf *newInfo);

  /**
   * @brief operator影响到的chunkServer列表, TransferLeader和RemovePeer的开销比较小,
   * 涉及到的chunkServer不在列; AddPeer中新增的peer需要拷贝数据,被认为Affected.
   *
   * @return set of affected chunkServers
   */
  std::vector<ChunkServerIdType> AffectedChunkServers() const;

  bool IsTimeout();

  std::string OpToString() const;

 public:
  EpochType startEpoch;
  // CopySetKey is a pair, first-logicalPoolId, second-copysetId
  CopySetKey copysetID;
  steady_clock::time_point createTime;
  OperatorPriority priority;
  // TODO(lixiaocui): 可能可以改成模板
  std::shared_ptr<OperatorStep> step;
  steady_clock::duration timeLimit;
};
}  // namespace schedule
}  // namespace mds
}  // namespace curve
#endif  // SRC_MDS_SCHEDULE_OPERATOR_H_

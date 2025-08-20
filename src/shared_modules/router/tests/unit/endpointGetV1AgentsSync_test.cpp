/*
 * Wazuh router
 * Copyright (C) 2015, Wazuh Inc.
 * May 5, 2025.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "shared_modules/router/src/wazuh-db/endpointGetV1AgentsSync.hpp"
#include "shared_modules/utils/mocks/sqlite3WrapperMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Sequence;

/**
 * @brief Tests for the EndpointGetV1AgentsSync class.
 */
class EndpointGetV1AgentsSyncTest : public ::testing::Test
{
protected:
    /**
     * @brief Set up the test fixture.
     */
    void SetUp() override
    {
        stmt = std::make_shared<NiceMock<MockSQLiteStatement>>();
        queries = std::make_shared<std::vector<std::string>>();
        TrampolineSQLiteStatement::inject(stmt, queries);

        ON_CALL(*stmt, bindStringView)
            .WillByDefault(
                [](int, std::string_view)
                {
                    // Implement bindStringView logic here.
                });
        ON_CALL(*stmt, bindString)
            .WillByDefault(
                [](int, const std::string&)
                {
                    // Implement bindString logic here.
                });
        ON_CALL(*stmt, bindInt64)
            .WillByDefault(
                [](int, int64_t)
                {
                    // Implement bindInt64 logic here.
                });
        ON_CALL(*stmt, reset)
            .WillByDefault(
                []
                {
                    // Implement reset logic here.
                });

        ON_CALL(*stmt, valueString).WillByDefault(Return("value"));
    }

    /**
     * @brief Tear down the test fixture.
     */
    void TearDown() override
    {
        TrampolineSQLiteStatement::inject(nullptr, nullptr);
    }

public:
    std::shared_ptr<NiceMock<MockSQLiteStatement>> stmt; ///< SQLite statement mock
    std::shared_ptr<std::vector<std::string>> queries;   ///< SQLite queries mock
};

TEST_F(EndpointGetV1AgentsSyncTest, HappyPath)
{
    Sequence s;
    EXPECT_CALL(*stmt, step())
        .InSequence(s)
        // COUNT #1
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        // SELECT syncreq and LABELS
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        .WillOnce(Return(SQLITE_DONE))
        // COUNT #2
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        // SELECT keepalive
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        // COUNT #3
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        // SELECT status
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        // UPDATE
        .WillOnce(Return(SQLITE_DONE));

    ON_CALL(*stmt, valueInt64).WillByDefault(Return(1));

    MockSQLiteConnection db;
    httplib::Request req;
    httplib::Response res;

    TEndpointGetV1AgentsSync<MockSQLiteConnection, TrampolineSQLiteStatement>::call(db, req, res);

    EXPECT_EQ(
        res.body,
        R"({"syncreq":[{"id":1,"name":"value","ip":"value","os_name":"value","os_version":"value","os_major":"value","os_minor":"value","os_codename":"value","os_build":"value","os_platform":"value","os_uname":"value","os_arch":"value","version":"value","config_sum":"value","merged_sum":"value","manager_host":"value","node_name":"value","last_keepalive":1,"connection_status":"value","disconnection_time":1,"group_config_status":"value","status_code":1,"labels":[{"key":"value","value":"value"}]}],"syncreq_keepalive":[{"id":1,"version":"value"}],"syncreq_status":[{"id":1,"version":"value","connection_status":"value","disconnection_time":1,"status_code":1}]})");

    ASSERT_EQ(queries->size(), 6); // 1 COUNT + 3 SELECT + 1 LABEL + 1 UPDATE
    EXPECT_EQ((*queries)[0], "SELECT COUNT(*) FROM agent WHERE id > 0 AND sync_status = ?;");

    EXPECT_EQ(
        (*queries)[1],
        "SELECT id, name, ip, os_name, os_version, os_major, os_minor, os_codename, os_build, os_platform, os_uname, "
        "os_arch, version, config_sum, merged_sum, manager_host, node_name, last_keepalive, connection_status, "
        "disconnection_time, group_config_status, status_code FROM agent WHERE id > 0 AND sync_status = 'syncreq';");
    EXPECT_EQ((*queries)[2], "SELECT key, value FROM labels WHERE id = ?;");
    EXPECT_EQ((*queries)[3], "SELECT id, version FROM agent WHERE id > 0 AND sync_status = 'syncreq_keepalive';");
    EXPECT_EQ((*queries)[4],
              "SELECT id, version, connection_status, disconnection_time, status_code FROM agent WHERE id > 0 AND "
              "sync_status = "
              "'syncreq_status';");
    EXPECT_EQ((*queries)[5], "UPDATE agent SET sync_status = 'synced' WHERE id > 0;");
}

TEST_F(EndpointGetV1AgentsSyncTest, NoResults)
{
    Sequence s;
    EXPECT_CALL(*stmt, step())
        .InSequence(s)
        // COUNT #1
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        // SELECT syncreq   (sin filas)
        .WillOnce(Return(SQLITE_DONE))
        // COUNT #2
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        // SELECT keepalive (sin filas)
        .WillOnce(Return(SQLITE_DONE))
        // COUNT #3
        .WillOnce(Return(SQLITE_ROW))
        .WillOnce(Return(SQLITE_DONE))
        // SELECT status    (sin filas)
        .WillOnce(Return(SQLITE_DONE))
        // UPDATE
        .WillOnce(Return(SQLITE_DONE));

    ON_CALL(*stmt, valueInt64).WillByDefault(Return(0));

    MockSQLiteConnection db;
    httplib::Request req;
    httplib::Response res;

    TEndpointGetV1AgentsSync<MockSQLiteConnection, TrampolineSQLiteStatement>::call(db, req, res);

    EXPECT_EQ(res.body, R"({})");

    ASSERT_EQ(queries->size(), 6);
}

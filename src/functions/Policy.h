/**
 * Created March 18, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef IAM_POLICY_H
#define IAM_POLICY_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/JSON.h"
#include "./core/ObjectWriter.h"
#include "./core/AsyncClient/AsyncClient.h"
#include "./core/URL.h"

// https://cloud.google.com/functions/docs/reference/rest/Shared.Types/Policy

#if defined(ENABLE_FUNCTIONS)

namespace IAMPolicy
{
    // The list of valid permission types for which logging can be configured. Admin writes are always logged, and are not configurable.
    enum LogType
    {
        LOG_TYPE_UNSPECIFIED, //	Default case. Should never be this.
        ADMIN_READ,           //	Admin reads. Example: CloudIAM getIamPolicy
        DATA_WRITE,           //	Data writes. Example: CloudSQL Users create
        DATA_READ             //	Data reads. Example: CloudSQL Users list
    };
    /**
     * Represents a textual expression in the Common Expression Language (CEL) syntax. CEL is a C-like expression language. The syntax and semantics of CEL are documented at https://github.com/google/cel-spec.
     */
    struct Expr : public Printable
    {
    protected:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;
        URLHelper uh;

        Expr &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Textual representation of an expression in Common Expression Language syntax.
        Expr &expression(const String &value) { return setObject(buf[1], FPSTR("expression"), value, true, true); }
        // Optional. Title for the expression, i.e. a short string describing its purpose. This can be used e.g. in UIs which allow to enter the expression.
        Expr &title(const String &value) { return setObject(buf[2], FPSTR("title"), value, true, true); }
        // Optional. Description of the expression. This is a longer text which describes the expression, e.g. when hovered over it in a UI.
        Expr &description(const String &value) { return setObject(buf[3], FPSTR("description"), value, true, true); }
        // Optional. String indicating the location of the expression for error reporting, e.g. a file name and a position in the file.
        Expr &location(const String &value) { return setObject(buf[4], FPSTR("location"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Associates members, or principals, with a role.
     */
    struct Binding : public Printable
    {
    protected:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        URLHelper uh;

        Binding &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Role that is assigned to the list of members, or principals. For example, roles/viewer, roles/editor, or roles/owner.
        // For an overview of the IAM roles and permissions, see the IAM documentation (https://cloud.google.com/iam/docs/roles-overview).
        // For a list of the available pre-defined roles, see here https://cloud.google.com/iam/docs/understanding-roles.
        Binding &role(const String &value) { return setObject(buf[1], FPSTR("role"), value, true, true); }
        // Specifies the principals requesting access for a Google Cloud resource. members can have the following values:
        /**
         * allUsers: A special identifier that represents anyone who is on the internet; with or without a Google account.
         * allAuthenticatedUsers: A special identifier that represents anyone who is authenticated with a Google account or a service account. Does not include identities that come from external identity providers (IdPs) through identity federation.
         * user:{emailid}: An email address that represents a specific Google account. For example, alice@example.com .
         * serviceAccount:{emailid}: An email address that represents a Google service account. For example, my-other-app@appspot.gserviceaccount.com.
         * serviceAccount:{projectid}.svc.id.goog[{namespace}/{kubernetes-sa}]: An identifier for a Kubernetes service account. For example, my-project.svc.id.goog[my-namespace/my-kubernetes-sa].
         * group:{emailid}: An email address that represents a Google group. For example, admins@example.com.
         * domain:{domain}: The G Suite domain (primary) that represents all the users of that domain. For example, google.com or example.com.
         * principal://iam.googleapis.com/locations/global/workforcePools/{pool_id}/subject/{subject_attribute_value}: A single identity in a workforce identity pool.
         * principalSet://iam.googleapis.com/locations/global/workforcePools/{pool_id}/group/{groupId}: All workforce identities in a group.
         * principalSet://iam.googleapis.com/locations/global/workforcePools/{pool_id}/attribute.{attribute_name}/{attribute_value}: All workforce identities with a specific attribute value.
         * principalSet://iam.googleapis.com/locations/global/workforcePools/{pool_id}/*: All identities in a workforce identity pool.
         * principal://iam.googleapis.com/projects/{project_number}/locations/global/workloadIdentityPools/{pool_id}/subject/{subject_attribute_value}: A single identity in a workload identity pool.
         * principalSet://iam.googleapis.com/projects/{project_number}/locations/global/workloadIdentityPools/{pool_id}/group/{groupId}: A workload identity pool group.
         * principalSet://iam.googleapis.com/projects/{project_number}/locations/global/workloadIdentityPools/{pool_id}/attribute.{attribute_name}/{attribute_value}: All identities in a workload identity pool with a certain attribute.
         * principalSet://iam.googleapis.com/projects/{project_number}/locations/global/workloadIdentityPools/{pool_id}/*: All identities in a workload identity pool.
         * deleted:user:{emailid}?uid={uniqueid}: An email address (plus unique identifier) representing a user that has been recently deleted. For example, alice@example.com?uid=123456789012345678901. If the user is recovered, this value reverts to user:{emailid} and the recovered user retains the role in the binding.
         * deleted:serviceAccount:{emailid}?uid={uniqueid}: An email address (plus unique identifier) representing a service account that has been recently deleted. For example, my-other-app@appspot.gserviceaccount.com?uid=123456789012345678901. If the service account is undeleted, this value reverts to serviceAccount:{emailid} and the undeleted service account retains the role in the binding.
         * deleted:group:{emailid}?uid={uniqueid}: An email address (plus unique identifier) representing a Google group that has been recently deleted. For example, admins@example.com?uid=123456789012345678901. If the group is recovered, this value reverts to group:{emailid} and the recovered group retains the role in the binding.
         * deleted:principal://iam.googleapis.com/locations/global/workforcePools/{pool_id}/subject/{subject_attribute_value}: Deleted single identity in a workforce identity pool. For example, deleted:principal://iam.googleapis.com/locations/global/workforcePools/my-pool-id/subject/my-subject-attribute-value.
         */
        Binding &members(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[2], FPSTR("members"), value.c_str(), false);
            return *this;
        }
        // The condition that is associated with this binding.
        // If the condition evaluates to true, then this binding applies to the current request.
        // If the condition evaluates to false, then this binding does not apply to the current request. However, a different role binding might grant the same role to one or more of the principals in this binding.
        // To learn which resources support conditions in their IAM policies, see the IAM documentation (https://cloud.google.com/iam/help/conditions/resource-policies).
        Binding &condition(const Expr &value) { return setObject(buf[3], FPSTR("condition"), value.c_str(), false, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Provides the configuration for logging a type of permissions. Example:
     */
    struct AuditLogConfig : public Printable
    {
    protected:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        URLHelper uh;

        AuditLogConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Specifies a service that will be enabled for audit logging. For example, storage.googleapis.com, cloudsql.googleapis.com. allServices is a special value that covers all services.
        AuditLogConfig &logType(LogType value)
        {
            if (value == LOG_TYPE_UNSPECIFIED)
                return setObject(buf[1], "logType", "LOG_TYPE_UNSPECIFIED", true, true);
            else if (value == ADMIN_READ)
                return setObject(buf[1], "logType", "ADMIN_READ", true, true);
            else if (value == DATA_WRITE)
                return setObject(buf[1], "logType", "DATA_WRITE", true, true);
            else if (value == DATA_READ)
                return setObject(buf[1], "logType", "DATA_READ", true, true);
            return *this;
        }
        // The configuration for logging of each type of permission.
        AuditLogConfig &exemptedMembers(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[2], FPSTR("exemptedMembers"), value.c_str(), false);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * Specifies the audit configuration for a service. The configuration determines which permission types are logged, and what identities, if any, are exempted from logging. An AuditConfig must have one or more AuditLogConfigs.
     */
    struct AuditConfig : public Printable
    {
    protected:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;
        URLHelper uh;

        AuditConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Specifies a service that will be enabled for audit logging. For example, storage.googleapis.com, cloudsql.googleapis.com. allServices is a special value that covers all services.
        AuditConfig &service(const String &value) { return setObject(buf[1], FPSTR("service"), value, true, true); }
        // The configuration for logging of each type of permission.
        AuditConfig &auditLogConfigs(const AuditLogConfig &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[2], FPSTR("auditLogConfigs"), value.c_str(), false);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
    /**
     * An Identity and Access Management (IAM) policy, which specifies access controls for Google Cloud resources.
     * A Policy is a collection of bindings. A binding binds one or more members, or principals, to a single role.
     */
    struct Policy : public Printable
    {
    protected:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;
        URLHelper uh;

        Policy &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Specifies the format of the policy.
        // Valid values are 0, 1, and 3. Requests that specify an invalid value are rejected.
        Policy &version(uint32_t value) { return setObject(buf[1], FPSTR("version"), String(value), false, true); }
        // Associates a list of members, or principals, with a role. Optionally, may specify a condition that determines how and when the bindings are applied. Each of the bindings must contain at least one principal.
        Policy &bindings(const Binding &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[2], FPSTR("bindings"), value.c_str(), false);
            return *this;
        }
        // pecifies cloud audit logging configuration for this policy.
        Policy &auditConfigs(const AuditConfig &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[3], FPSTR("auditConfigs"), value.c_str(), false);
            return *this;
        }
        // etag is used for optimistic concurrency control as a way to help prevent simultaneous updates of a policy from overwriting each other. It is strongly suggested that systems make use of the etag in the read-modify-write cycle to perform policy updates in order to avoid race conditions: An etag is returned in the response to getIamPolicy, and systems are expected to put that etag in the request to setIamPolicy to ensure that their change will be applied to the same version of the policy.
        Policy &etag(const String &value) { return setObject(buf[4], FPSTR("etag"), value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

}

#endif

#endif

#include "pathplanner/lib/auto/AutoBuilder.h"
#include "pathplanner/lib/commands/FollowPathHolonomic.h"
#include "pathplanner/lib/commands/FollowPathRamsete.h"
#include "pathplanner/lib/commands/FollowPathLTV.h"
#include "pathplanner/lib/commands/FollowPathWithEvents.h"
#include "pathplanner/lib/auto/CommandUtil.h"
#include <stdexcept>
#include <frc2/command/Commands.h>
#include <frc/Filesystem.h>
#include <wpi/raw_istream.h>

using namespace pathplanner;

bool AutoBuilder::m_configured = false;
std::function<frc2::CommandPtr(std::shared_ptr<PathPlannerPath>)> AutoBuilder::m_pathFollowingCommandBuilder;
std::function<frc::Pose2d()> AutoBuilder::m_getPose;
std::function<void(frc::Pose2d)> AutoBuilder::m_resetPose;

void AutoBuilder::configureHolonomic(std::function<frc::Pose2d()> poseSupplier,
		std::function<void(frc::Pose2d)> resetPose,
		std::function<frc::ChassisSpeeds()> robotRelativeSpeedsSupplier,
		std::function<void(frc::ChassisSpeeds)> robotRelativeOutput,
		HolonomicPathFollowerConfig config, frc2::Subsystem *driveSubsystem) {
	if (m_configured) {
		throw std::runtime_error(
				"Auto builder has already been configured. Please only configure auto builder once");
	}

	AutoBuilder::m_pathFollowingCommandBuilder = [poseSupplier,
			robotRelativeSpeedsSupplier, robotRelativeOutput, config,
			driveSubsystem](std::shared_ptr<PathPlannerPath> path) {
		return FollowPathHolonomic(path, poseSupplier,
				robotRelativeSpeedsSupplier, robotRelativeOutput, config, {
						driveSubsystem }).ToPtr();
	};
	AutoBuilder::m_getPose = poseSupplier;
	AutoBuilder::m_resetPose = resetPose;
	AutoBuilder::m_configured = true;
}

void AutoBuilder::configureRamsete(std::function<frc::Pose2d()> poseSupplier,
		std::function<void(frc::Pose2d)> resetPose,
		std::function<frc::ChassisSpeeds()> speedsSupplier,
		std::function<void(frc::ChassisSpeeds)> output,
		frc2::Subsystem *driveSubsystem) {
	if (m_configured) {
		throw std::runtime_error(
				"Auto builder has already been configured. Please only configure auto builder once");
	}

	AutoBuilder::m_pathFollowingCommandBuilder = [poseSupplier, speedsSupplier,
			output, driveSubsystem](std::shared_ptr<PathPlannerPath> path) {
		return FollowPathRamsete(path, poseSupplier, speedsSupplier, output, {
				driveSubsystem }).ToPtr();
	};
	AutoBuilder::m_getPose = poseSupplier;
	AutoBuilder::m_resetPose = resetPose;
	AutoBuilder::m_configured = true;
}

void AutoBuilder::configureRamsete(std::function<frc::Pose2d()> poseSupplier,
		std::function<void(frc::Pose2d)> resetPose,
		std::function<frc::ChassisSpeeds()> speedsSupplier,
		std::function<void(frc::ChassisSpeeds)> output,
		units::unit_t<frc::RamseteController::b_unit> b,
		units::unit_t<frc::RamseteController::zeta_unit> zeta,
		frc2::Subsystem *driveSubsystem) {
	if (m_configured) {
		throw std::runtime_error(
				"Auto builder has already been configured. Please only configure auto builder once");
	}

	AutoBuilder::m_pathFollowingCommandBuilder = [poseSupplier, speedsSupplier,
			output, b, zeta, driveSubsystem](
			std::shared_ptr<PathPlannerPath> path) {
		return FollowPathRamsete(path, poseSupplier, speedsSupplier, output, b,
				zeta, { driveSubsystem }).ToPtr();
	};
	AutoBuilder::m_getPose = poseSupplier;
	AutoBuilder::m_resetPose = resetPose;
	AutoBuilder::m_configured = true;
}

void AutoBuilder::configureLTV(std::function<frc::Pose2d()> poseSupplier,
		std::function<void(frc::Pose2d)> resetPose,
		std::function<frc::ChassisSpeeds()> speedsSupplier,
		std::function<void(frc::ChassisSpeeds)> output,
		const wpi::array<double, 3> &Qelms, const wpi::array<double, 2> &Relms,
		units::second_t dt, frc2::Subsystem *driveSubsystem) {
	if (m_configured) {
		throw std::runtime_error(
				"Auto builder has already been configured. Please only configure auto builder once");
	}

	AutoBuilder::m_pathFollowingCommandBuilder = [poseSupplier, speedsSupplier,
			output, Qelms, Relms, dt, driveSubsystem](
			std::shared_ptr<PathPlannerPath> path) {
		return FollowPathLTV(path, poseSupplier, speedsSupplier, output, Qelms,
				Relms, dt, { driveSubsystem }).ToPtr();
	};
	AutoBuilder::m_getPose = poseSupplier;
	AutoBuilder::m_resetPose = resetPose;
	AutoBuilder::m_configured = true;
}

void AutoBuilder::configureLTV(std::function<frc::Pose2d()> poseSupplier,
		std::function<void(frc::Pose2d)> resetPose,
		std::function<frc::ChassisSpeeds()> speedsSupplier,
		std::function<void(frc::ChassisSpeeds)> output, units::second_t dt,
		frc2::Subsystem *driveSubsystem) {
	if (m_configured) {
		throw std::runtime_error(
				"Auto builder has already been configured. Please only configure auto builder once");
	}

	AutoBuilder::m_pathFollowingCommandBuilder = [poseSupplier, speedsSupplier,
			output, dt, driveSubsystem](std::shared_ptr<PathPlannerPath> path) {
		return FollowPathLTV(path, poseSupplier, speedsSupplier, output, dt, {
				driveSubsystem }).ToPtr();
	};
	AutoBuilder::m_getPose = poseSupplier;
	AutoBuilder::m_resetPose = resetPose;
	AutoBuilder::m_configured = true;
}

void AutoBuilder::configureCustom(
		std::function<frc2::CommandPtr(std::shared_ptr<PathPlannerPath>)> pathFollowingCommandBuilder,
		std::function<frc::Pose2d()> poseSupplier,
		std::function<void(frc::Pose2d)> resetPose) {
	if (m_configured) {
		throw std::runtime_error(
				"Auto builder has already been configured. Please only configure auto builder once");
	}

	AutoBuilder::m_pathFollowingCommandBuilder = pathFollowingCommandBuilder;
	AutoBuilder::m_getPose = poseSupplier;
	AutoBuilder::m_resetPose = resetPose;
	AutoBuilder::m_configured = true;
}

frc2::CommandPtr AutoBuilder::followPathWithEvents(
		std::shared_ptr<PathPlannerPath> path) {
	if (!m_configured) {
		throw std::runtime_error(
				"Auto builder was used to build a path following command before being configured");
	}

	return FollowPathWithEvents(m_pathFollowingCommandBuilder(path).Unwrap(),
			path, m_getPose).ToPtr();
}

frc2::CommandPtr AutoBuilder::buildAuto(std::string autoName) {
	const std::string filePath = frc::filesystem::GetDeployDirectory()
			+ "/pathplanner/autos/" + autoName + ".auto";

	std::error_code error_code;
	wpi::raw_fd_istream input { filePath, error_code };

	if (error_code) {
		throw std::runtime_error("Cannot open file: " + filePath);
	}

	wpi::json json;
	input >> json;

	return getAutoCommandFromJson(json);
}

frc2::CommandPtr AutoBuilder::getAutoCommandFromJson(const wpi::json &json) {
	wpi::json::const_reference commandJson = json.at("command");

	frc2::CommandPtr autoCommand = CommandUtil::commandFromJson(commandJson);
	if (json.find("startingPose") != json.end()) {
		frc::Pose2d startPose = getStartingPoseFromJson(
				json.at("startingPose"));
		return frc2::cmd::Sequence(frc2::cmd::RunOnce([startPose]() {
			m_resetPose(startPose);
		}), std::move(autoCommand));
	} else {
		return autoCommand;
	}
}

frc::Pose2d AutoBuilder::getStartingPoseFromJson(const wpi::json &json) {
	wpi::json::const_reference pos = json.at("position");
	units::meter_t x = units::meter_t { pos.at("x").get<double>() };
	units::meter_t y = units::meter_t { pos.at("y").get<double>() };
	units::degree_t deg = units::degree_t { json.at("rotation").get<double>() };

	return frc::Pose2d(x, y, frc::Rotation2d(deg));
}
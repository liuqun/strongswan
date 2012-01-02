/*
 * Copyright (C) 2006 Martin Willi
 * Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "delete_child_sa_job.h"

#include <daemon.h>


typedef struct private_delete_child_sa_job_t private_delete_child_sa_job_t;

/**
 * Private data of an delete_child_sa_job_t object.
 */
struct private_delete_child_sa_job_t {
	/**

	 * Public delete_child_sa_job_t interface.
	 */
	delete_child_sa_job_t public;

	/**
	 * reqid of the CHILD_SA
	 */
	u_int32_t reqid;

	/**
	 * protocol of the CHILD_SA (ESP/AH)
	 */
	protocol_id_t protocol;

	/**
	 * inbound SPI of the CHILD_SA
	 */
	u_int32_t spi;

	/**
	 * Delete for an expired CHILD_SA
	 */
	bool expired;
};

METHOD(job_t, destroy, void,
	private_delete_child_sa_job_t *this)
{
	free(this);
}

METHOD(job_t, execute, void,
	private_delete_child_sa_job_t *this)
{
	ike_sa_t *ike_sa;

	ike_sa = charon->ike_sa_manager->checkout_by_id(charon->ike_sa_manager,
													this->reqid, TRUE);
	if (ike_sa == NULL)
	{
		DBG1(DBG_JOB, "CHILD_SA with reqid %d not found for delete",
			 this->reqid);
	}
	else
	{
		ike_sa->delete_child_sa(ike_sa, this->protocol, this->spi, this->expired);

		charon->ike_sa_manager->checkin(charon->ike_sa_manager, ike_sa);
	}
	destroy(this);
}

METHOD(job_t, get_priority, job_priority_t,
	private_delete_child_sa_job_t *this)
{
	return JOB_PRIO_MEDIUM;
}

/*
 * Described in header
 */
delete_child_sa_job_t *delete_child_sa_job_create(u_int32_t reqid,
							protocol_id_t protocol, u_int32_t spi, bool expired)
{
	private_delete_child_sa_job_t *this;

	INIT(this,
		.public = {
			.job_interface = {
				.execute = _execute,
				.get_priority = _get_priority,
				.destroy = _destroy,
			},
		},
		.reqid = reqid,
		.protocol = protocol,
		.spi = spi,
		.expired = expired,
	);

	return &this->public;
}


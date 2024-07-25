; SPDX-License-Identifier: BSD-3-Clause
; Copyright(c) 2022 Intel Corporation

;
; Pipeline packet mirroring.
;
mirroring slots 1 sessions 64

;
; Pipeline input ports.
;
; Note: Customize the parameters below to match your setup.
;
port in 0 ethdev 0000:00:04.0 rxq 0 bsz 32

;
; Pipeline output ports.
;
; Note: Customize the parameters below to match your setup.
;
port out 0 ethdev 0000:00:04.0 txq 0 bsz 32

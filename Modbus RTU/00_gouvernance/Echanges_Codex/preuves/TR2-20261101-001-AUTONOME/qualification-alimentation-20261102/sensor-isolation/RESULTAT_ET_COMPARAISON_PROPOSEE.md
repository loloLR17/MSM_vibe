# D01 : diagnostic sans carte ni activité SD sur TR2-001

Sourceacb142794570ded5abd15f6521eb5d8b644590e8, profil capteur/FIFO/FIR16g8MHz,
60s maximum, départ USER unique, mêmes gardes horloge/budgets. Mode supplémentaire
désactivé par défaut ; interdit hors001 ou au-delà60s ; pas de montage/initSD,
pilote noninitialisé/écritures verrouillées. BuildARM du mode et du profil normalPASS,
configurationslongue/autrecible refusées. Ce n'est pas un mode de stockage de campagne.

Douzième flash standard de mission66-v/-rst, après identité65 ; BIN50912B
6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2,
readback67exact, WHO7B/FS04/calibration54µs, WAIT. SDphysiquementauPC confirméuser.
GDB68arméauWAIT, USERphysique confirmé, aucunhalt avant breakpointterminal.

Échec réel16,659203sMCU/error3/horloge,167checks. FIFO1489062/précédentdirect1481141,
directfautif19689725/bracket178243551–178243569 ; tracefd702c01 confirme le mot reçu.
Recalcul exact du résidu228049169µs/limite100. SnapshotCRC923a838d conforme,
source acb14279. Budgets complets sous limites/FIFO0/stoppedtrue, SDcommands/reads/writes0,
initializedfalse/write_enabledfalse, sensor_only1, aucunfichier. 44288sortiescalculées,
digestRAMseulement : sorties complètes nonarchivées, pas de récupération intégrale revendiquée.
Le breakout SD et ses fils restent dans le montage : ne pas exclure tous effets électriques.

Conclusion : la carte et son trafic ne sont pas des conditions nécessaires à cette
anomalie de lecture directe. La qualité globale de la512Go et le refus SD A1D restent
des sujets distincts. Aucun échange SD32 ni aucune action TR2-002 effectué.

Comparaison002 proposée, NON AUTORISÉE/NON EXÉCUTÉE à ce point :
- inventaire précis et sauvegarde Flash initiale vérifiée avant tout flash ;
- diagnostic sansSD dédié à l'UID002, méthode/FS/vitesse/contrôles comparables ;
- maximum2essais60s, source de données et câblage tracés ;
- aucune modification des SD/FRAM ; retour à l'image Flash initiale vérifiée ;
- une différence entre bancs ne prouvera pas à elle seule une Nucleo001 défectueuse.
Le contrat en cours impose cible001 et interdit002 ; extension humaine explicite
requise avant toute session ou manipulation de002 (AGENTS§1/§8 etMISSION_EN_COURS).
Aucune modification de MISSION_EN_COURS.md par Codex.

## CN0 — nouvel essai comparatif CN15 seul, chargé mais NON démarré

Demande utilisateur explicite après GI0 : nouvelle acquisition2h sans ST-LINK. Nom **CN00005.TR2** confirmé absent sur la SD512 FAT32/cluster32768, capacité/partition exactes,509038264320octets libres. Aucun fichier effacé/modifié, GI0 et anciens fichiers conservés.

Code source `6633b0154bf67b4c5437f1d259732fc1b1f21e4a`, cross-build ciblé ARM PASS, BIN53340octets/SHA `d31863809d62af9fc690e5ed1e8ef5fa2c3172c764774f0cb51f5e8e335cd396`. **Comparaison intégrale des BIN GI0/CN0 : seuls le préfixe GI0→CN0 et le SHA source embarqué diffèrent.** Durée7200000000µsMCU, allocation640MiB/±16g/488µgLSB/FIR107dec10/SPI3 8MHz/SD4MHz, tous contrôles qualité inchangés. Aucun ancien test rejoué ; compilation requise pour ce nouveau nom, aucune modification fonctionnelle.

Utilisateur confirme CN15+STLINK connectés/lecteur001vide/512auPC/JP6USB-C/JP2retiré. Préflash UID001/SN003500463235510B37333439/capteurwriteroff/nonincertain, programmationstandard à08000000 `-w -v -rst` réussie. Boot sanscarte stage99/error2/WHO7B attendu, SDwrites0/FIFOoff/writeroff/CShaute/GDBdétaché. Aucun START ni nouveau test effectué. Pas de masserase/OB/FRAM/COM4/002/protection modifiée.

**Étape physique en attente :** éjectionPC, coupureCN15+STLINK10s, réinsertion512, CN15secteurseul/STLINKdébranché, attendre WAITvert sansUSER/RESET. Puis USERphysique au signal, laisserCN15pendantcapture, noterheure PC. Ne pas connecter GDB pendantcapture. Àla fin conserverCN15 avantreconnexionSTLINK pourrecueillirRAM/digest/snapshot sansresetflash ; la reconnexion peut provoquer un reboot malgréJP2retiré (limite déjàobservée), snapshotNOLOAD utile mais non garanti après coupure. Aucune2hCN0 qualifiée ni départ réel revendiqués.

GI0 demeure : deuxheures atteintes/tousXYZ égauxdigestRAM mais échecqualitéfinalclips1 ; mouvement/choc volontaire possiblement rapporté paruser sanshorodatage, mitigation différée. Données/proofs précédentes préservées, suitephysiqueuserenattente.

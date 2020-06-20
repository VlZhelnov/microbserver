--
-- PostgreSQL database dump
--

-- Dumped from database version 11.7 (Raspbian 11.7-0+deb10u1)
-- Dumped by pg_dump version 11.7 (Raspbian 11.7-0+deb10u1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: entry; Type: TABLE; Schema: public; Owner: pi
--

CREATE TABLE public.entry (
    id integer NOT NULL,
    elapsed_time integer NOT NULL,
    temperature smallint NOT NULL,
    illumination smallint NOT NULL,
    microrequest_id integer NOT NULL
);


ALTER TABLE public.entry OWNER TO pi;

--
-- Name: entry_id_seq; Type: SEQUENCE; Schema: public; Owner: pi
--

CREATE SEQUENCE public.entry_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.entry_id_seq OWNER TO pi;

--
-- Name: entry_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: pi
--

ALTER SEQUENCE public.entry_id_seq OWNED BY public.entry.id;


--
-- Name: microrequest; Type: TABLE; Schema: public; Owner: pi
--

CREATE TABLE public.microrequest (
    id integer NOT NULL,
    title character varying(200) NOT NULL,
    data_request timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    data_accept timestamp without time zone,
    delay integer NOT NULL,
    quantity integer NOT NULL,
    status character varying(10) DEFAULT 'adopted'::character varying NOT NULL,
    data_completed timestamp without time zone,
    CONSTRAINT microrequest_delay_check CHECK (((delay >= 0) AND (delay <= 99999))),
    CONSTRAINT microrequest_quantity_check CHECK (((quantity >= 0) AND (quantity <= 99999))),
    CONSTRAINT microrequest_status_check CHECK (((status)::text = ANY ((ARRAY['adopted'::character varying, 'processing'::character varying, 'error'::character varying, 'completed'::character varying])::text[])))
);


ALTER TABLE public.microrequest OWNER TO pi;

--
-- Name: microrequest_id_seq; Type: SEQUENCE; Schema: public; Owner: pi
--

CREATE SEQUENCE public.microrequest_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.microrequest_id_seq OWNER TO pi;

--
-- Name: microrequest_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: pi
--

ALTER SEQUENCE public.microrequest_id_seq OWNED BY public.microrequest.id;


--
-- Name: entry id; Type: DEFAULT; Schema: public; Owner: pi
--

ALTER TABLE ONLY public.entry ALTER COLUMN id SET DEFAULT nextval('public.entry_id_seq'::regclass);


--
-- Name: microrequest id; Type: DEFAULT; Schema: public; Owner: pi
--

ALTER TABLE ONLY public.microrequest ALTER COLUMN id SET DEFAULT nextval('public.microrequest_id_seq'::regclass);


--
-- Data for Name: entry; Type: TABLE DATA; Schema: public; Owner: pi
--

COPY public.entry (id, elapsed_time, temperature, illumination, microrequest_id) FROM stdin;
\.


--
-- Data for Name: microrequest; Type: TABLE DATA; Schema: public; Owner: pi
--

COPY public.microrequest (id, title, data_request, data_accept, delay, quantity, status, data_completed) FROM stdin;
\.


--
-- Name: entry_id_seq; Type: SEQUENCE SET; Schema: public; Owner: pi
--

SELECT pg_catalog.setval('public.entry_id_seq', 0, true);


--
-- Name: microrequest_id_seq; Type: SEQUENCE SET; Schema: public; Owner: pi
--

SELECT pg_catalog.setval('public.microrequest_id_seq', 0, true);


--
-- Name: entry entry_pkey; Type: CONSTRAINT; Schema: public; Owner: pi
--

ALTER TABLE ONLY public.entry
    ADD CONSTRAINT entry_pkey PRIMARY KEY (id);


--
-- Name: microrequest microrequest_pkey; Type: CONSTRAINT; Schema: public; Owner: pi
--

ALTER TABLE ONLY public.microrequest
    ADD CONSTRAINT microrequest_pkey PRIMARY KEY (id);


--
-- Name: entry entry_microrequest_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: pi
--

ALTER TABLE ONLY public.entry
    ADD CONSTRAINT entry_microrequest_id_fkey FOREIGN KEY (microrequest_id) REFERENCES public.microrequest(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

